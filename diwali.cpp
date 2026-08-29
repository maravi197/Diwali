
#include <ncurses.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std; 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Particle {
    double x, y;
    double vx, vy;
    int life;
    int max_life;
    int color;
    int shape;
    bool has_split;
};

struct Glow {
    double cx, cy;
    double radius;
    int life;
    int max_life;
};

static vector<Particle> particles;
static vector<Glow> glows;
static const int NUM_COLORS = 7;

static const char FADE_SETS[6][4] = {
    {'*', 'o', '.', ' '},
    {'+', 'x', '\'', ' '},
    {'#', '+', ':', ' '},
    {'@', 'o', '`', ' '},
    {'x', '.', ',', ' '},
    {'o', ':', '.', ' '}
};
static const int NUM_SHAPES = 6;

double randD() { return (double)rand() / RAND_MAX; }

void spawnExplosion(int max_x, int max_y) {
    if (max_x < 6 || max_y < 6) return;

    double cx = 3 + rand() % (max_x - 6);
    double cy = 2 + rand() % (max_y - 4);

    Glow g;
    g.cx = cx;
    g.cy = cy;
    g.radius = 2.0 + randD() * 2.0; 
    g.max_life = 6 + rand() % 6;    
    g.life = g.max_life;
    glows.push_back(g);

    int count = 50 + rand() % 40; 
    for (int i = 0; i < count; i++) {
        double angle = randD() * 2.0 * M_PI;
        double speed = 0.15 + randD() * 0.35;

        Particle p;
        p.x = cx;
        p.y = cy;
        p.vx = cos(angle) * speed * 1.6;
        p.vy = sin(angle) * speed * 0.8;
        p.max_life = 14 + rand() % 16;
        p.life = p.max_life;
        p.color = 1 + rand() % NUM_COLORS;
        p.shape = rand() % NUM_SHAPES;
        p.has_split = false;
        particles.push_back(p);
    }
}

void updateGlows() {
    for (auto &g : glows) g.life--;
    glows.erase(
        remove_if(glows.begin(), glows.end(),
            [](const Glow &g) { return g.life <= 0; }),
        glows.end()
    );
}

void updateParticles() {
    vector<Particle> crackleSparks;

    for (auto &p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.vy += 0.025;
        p.life--;

        double frac = (double)p.life / (double)p.max_life;

        if (frac > 0.65) {
            p.color = 1 + rand() % NUM_COLORS;
        }

        if (!p.has_split && frac < 0.55 && frac > 0.30 && (rand() % 1000 < 12)) {
            p.has_split = true;
            int mini = 3 + rand() % 5;
            for (int k = 0; k < mini; k++) {
                double angle = randD() * 2.0 * M_PI;
                double speed = 0.08 + randD() * 0.20;
                Particle np;
                np.x = p.x;
                np.y = p.y;
                np.vx = cos(angle) * speed * 1.6;
                np.vy = sin(angle) * speed * 0.8;
                np.max_life = 8 + rand() % 8;
                np.life = np.max_life;
                np.color = 1 + rand() % NUM_COLORS;
                np.shape = rand() % NUM_SHAPES;
                np.has_split = true;
                crackleSparks.push_back(np);
            }
        }
    }

    particles.insert(particles.end(), crackleSparks.begin(), crackleSparks.end());

    particles.erase(
        remove_if(particles.begin(), particles.end(),
            [](const Particle &p) { return p.life <= 0; }),
        particles.end()
    );
}

void drawGlows(int max_x, int max_y) {
    const int LIT_CHANCE = 45; 

    for (auto &g : glows) {
        int rx = (int)round(g.radius * 1.6);
        int ry = (int)round(g.radius * 0.8);
        if (rx < 1) rx = 1;
        if (ry < 1) ry = 1;

        int cxI = (int)round(g.cx);
        int cyI = (int)round(g.cy);

        for (int dy = -ry; dy <= ry; dy++) {
            for (int dx = -rx; dx <= rx; dx++) {
                double norm = ((double)(dx * dx)) / (double)(rx * rx)
                            + ((double)(dy * dy)) / (double)(ry * ry);
                if (norm > 1.0) continue;               
                if (rand() % 100 >= LIT_CHANCE) continue; 

                int px = cxI + dx;
                int py = cyI + dy;
                if (px < 0 || px >= max_x || py < 0 || py >= max_y) continue;

                int c = 1 + rand() % NUM_COLORS; 
                attron(COLOR_PAIR(c) | A_REVERSE);
                mvaddch(py, px, ' ');
                attroff(COLOR_PAIR(c) | A_REVERSE);
            }
        }
    }
}

void drawParticles(int max_x, int max_y) {
    for (auto &p : particles) {
        int ix = (int)round(p.x);
        int iy = (int)round(p.y);
        if (ix < 0 || ix >= max_x || iy < 0 || iy >= max_y) continue;

        double frac = (double)p.life / (double)p.max_life;
        int idx = (int)((1.0 - frac) * 3.0);
        if (idx < 0) idx = 0;
        if (idx > 3) idx = 3;
        char symbol = FADE_SETS[p.shape][idx];

        bool bright = frac > 0.6;
        attron(COLOR_PAIR(p.color));
        if (bright) attron(A_BOLD);
        mvaddch(iy, ix, symbol);
        if (bright) attroff(A_BOLD);
        attroff(COLOR_PAIR(p.color));
    }
}

int main() {
    srand((unsigned)time(nullptr));

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(3, COLOR_GREEN,   COLOR_BLACK);
        init_pair(4, COLOR_CYAN,    COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_WHITE,   COLOR_BLACK);
        init_pair(7, COLOR_BLUE,    COLOR_BLACK);
    }

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    spawnExplosion(max_x, max_y);
    spawnExplosion(max_x, max_y);
    spawnExplosion(max_x, max_y);

    while (true) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == KEY_RESIZE) {
            getmaxyx(stdscr, max_y, max_x);
        }

        if (rand() % 100 < 14) {
            int burst = 1 + rand() % 4;
            for (int i = 0; i < burst; i++) {
                spawnExplosion(max_x, max_y);
            }
        }

        updateGlows();
        updateParticles();

        erase();
        drawGlows(max_x, max_y);
        drawParticles(max_x, max_y);

        attron(COLOR_PAIR(6) | A_BOLD);
        attroff(COLOR_PAIR(6) | A_BOLD);

        refresh();
        napms(25);
    } 

    endwin();
    return 0;
}
