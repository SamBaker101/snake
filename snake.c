// Sam Baker - 2019
// cd C:\code\test\SDL-learning
// gcc snake.c -o snake -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

/////////////////////////////////////////////////////////////////
///                   Preprocessor                            ///
////////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 80
#define PILL_MAX 5

//this is a test

#define CELLCOUNT_X 40
#define CELLCOUNT_Y 30
#define CELL_W 24
#define CELL_H 24
#define BORDER_W 20
#define WINDOW_W ((CELLCOUNT_X * CELL_W) + (2 * BORDER_W))
#define WINDOW_H ((CELLCOUNT_Y * CELL_H) + (2 * BORDER_W))

#define GETX(x) (x*CELL_W + BORDER_W)
#define GETY(y) (y*CELL_H + BORDER_W)

/////////////////////////////////////////////////////////////////
///                       Global                              ///
////////////////////////////////////////////////////////////////

int randomizer = 7951;

struct tile{
    int x, y, snake_head, snake_tail, pill;
};

struct tile tilelist[CELLCOUNT_X][CELLCOUNT_Y];

struct character{
    int x, y, direction, length;
};

/////////////////////////////////////////////////////////////////
///                        Functions                         ///
////////////////////////////////////////////////////////////////


//// Random number ////
int randomnum(int range){
    unsigned int random1, random2, random3;

    srand(time(NULL));

    random2 = randomizer % range;
    randomizer = (((97+randomizer)*(89+randomizer)*rand()) %1000) /10;

    return random2;
}

//// Make Pills ////

int createPill(){

    int count = randomnum(PILL_MAX) + 1;
    int i, n = count;

    for (i = 0; i < n; i++){
        int randx = randomnum(CELLCOUNT_X);
            if (randx >= CELLCOUNT_X)
                randx--;
        int randy = randomnum(CELLCOUNT_Y);
            if (randy >= CELLCOUNT_Y)
                randy--;
            if ((tilelist[randx][randy].snake_tail != 0) || (tilelist[randx][randy].pill != 0))
                count--;
            else
                tilelist[randx][randy].pill = 1;
        //printf("count: %d, pcoord: %d, %d, st: %d \n", count, randx, randy, tilelist[randx][randy].snake_tail);
    }
    return count;
}               

///Set up board///

void clearBoard(){
    int i, j;

    for (i = 0; i <= CELLCOUNT_X; i++){
        for (j = 0; j <= CELLCOUNT_Y; j++){
            tilelist[i][j].x = GETX(i);
            tilelist[i][j].y = GETY(j);
            tilelist[i][j].snake_head = 0;
            tilelist[i][j].snake_tail = 0;
            tilelist[i][j].pill = 0;
        }
    }

}

//// Death ////

void death(struct character *Player, int *pillcount){
    Player->x = 0;
    Player->y = 0;
    Player->length = 1;
    Player->direction = 2;
    *pillcount = 0;
    
    clearBoard();
    
} 

////////////////////////////////////////////////////////////////
//                   KEYS                                     //
////////////////////////////////////////////////////////////////
void checkEvents(SDL_Window *window, struct character *Player, int *done, int *pause){

    SDL_Event event;

    while(SDL_PollEvent(&event) && event.key.repeat == 0){
        if (event.type == SDL_WINDOWEVENT_CLOSE){
                    if(window);{
                        SDL_DestroyWindow(window);
                        window = NULL;
                        *done = 1;
                    }
                }

        if (event.type == SDL_QUIT){
                    SDL_DestroyWindow(window);
                    window = NULL;
                    *done = 1;
                }

        if (event.type == SDL_KEYDOWN){
            switch(event.key.keysym.sym){

                case SDLK_ESCAPE:
                            SDL_DestroyWindow(window);
                            window = NULL;
                            *done = 1;
                        break;

                // N=1, E=2, S=3, W=4
                case SDLK_w:
                    if (Player->direction != 3)
                        Player->direction = 1;
                    break;

                case SDLK_s:
                    if (Player->direction != 1)
                        Player->direction = 3;
                    break;

                case SDLK_a:
                    if (Player->direction != 2)
                        Player->direction = 4;
                    break;

                case SDLK_d:
                    if (Player->direction != 4)
                        Player->direction = 2;
                    break;

                case SDLK_SPACE:
                    if (*pause == 0)
                        *pause = 1;
                    else
                        *pause = 0;


        }
    }

    }   
}

/////////////////////////////////////////////////////////////////
///                   Handle Movement                        ///
////////////////////////////////////////////////////////////////

int moveChar(struct character *Player, int *pillcount){
    int i, j;

    if((Player->x == 0) && (Player->direction == 4))
        death(Player, pillcount);
    else if((Player->y == 0) && (Player->direction == 1))
        death(Player, pillcount);
    else if((Player->x + 1 == CELLCOUNT_X) && (Player->direction == 2))
        death(Player, pillcount);
    else if((Player->y + 1 == CELLCOUNT_Y) && (Player->direction == 3))
        death(Player, pillcount);
    else{
        switch (Player->direction){
            case 1:
                Player->y = Player->y - 1;
                break;

            case 2:
                Player->x = Player->x + 1;
                break;

            case 3:
                Player->y = Player->y + 1;
                break;

            case 4:
                Player->x = Player->x - 1;
                break;
        };

        if (tilelist[Player->x][Player->y].snake_tail > 0)
            death(Player, pillcount);
        
        else{
            tilelist[Player->x][Player->y].snake_tail = 1;

            if (tilelist[Player->x][Player->y].pill == 1){
                tilelist[Player->x][Player->y].pill = 0;
                *pillcount = *pillcount - 1;
                Player->length++;
            }

            for (i = 0; i < CELLCOUNT_X; i++){
                for (j = 0; j < CELLCOUNT_Y; j++){

                    if (tilelist[i][j].snake_tail != 0){
                        if (tilelist[i][j].snake_tail > Player->length)
                            tilelist[i][j].snake_tail = 0;
                        else
                            tilelist[i][j].snake_tail++;
                        }
                    }
                }
            }
        }
}


/////////////////////////////////////////////////////////////////
///                        Text                              ///
////////////////////////////////////////////////////////////////

void writeText(SDL_Renderer *renderer, SDL_Rect *rect, int size, char text[]){
    TTF_Init();
    TTF_Font *sans =  TTF_OpenFont("Fonts/Sansation-Bold.ttf", size);
    SDL_Color White = {255, 0, 0};
    SDL_Surface* message_surface = TTF_RenderText_Solid(sans, text, White);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, message_surface);

    SDL_RenderCopy(renderer, message, NULL, rect);
    SDL_FreeSurface(message_surface);
    //SDL_DestroyTexture(message);

    TTF_Quit();
}

/////////////////////////////////////////////////////////////////
///                        Draw                              ///
////////////////////////////////////////////////////////////////

void drawScreen(SDL_Renderer *renderer, int *pause){
    int i, j;
    int menuw = 20*CELL_W;
    int menuh = 10*CELL_H;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_Rect screen = {BORDER_W, BORDER_W, 
                        (WINDOW_W - (2 * BORDER_W)), 
                        (WINDOW_H - (2 * BORDER_W))};

    SDL_RenderFillRect(renderer, &screen);

    for (i = 0; i < CELLCOUNT_X; i++){
        for (j = 0; j < CELLCOUNT_Y; j++){
            if (tilelist[i][j].snake_tail != 0){
                    SDL_Rect snake_tail = {tilelist[i][j].x, tilelist[i][j].y, CELL_W, CELL_H};
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &snake_tail);
                }

            if (tilelist[i][j].pill == 1){
                SDL_Rect pill = {tilelist[i][j].x, tilelist[i][j].y, CELL_W, CELL_H};
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &pill);
            }
        }
    }

    if (*pause){

        SDL_Rect pauseborder = {((WINDOW_W / 2) - (menuw / 2)), 
                                ((WINDOW_H / 2) - (menuh / 2)), 
                                menuw, menuh};

        SDL_Rect pausemenu = {((WINDOW_W / 2) - (menuw / 2) + BORDER_W), 
                            ((WINDOW_H / 2) - (menuh / 2) + BORDER_W),
                            (menuw - 2*BORDER_W), 
                            (menuh - 2*BORDER_W)};


        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pauseborder);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pausemenu);

        char pause_text[] = "Paused";
        writeText(renderer, &pausemenu, 50, pause_text);
    }

    SDL_RenderPresent(renderer);
}

/////////////////////////////////////////////////////////////////
///                        Menues                            ///
////////////////////////////////////////////////////////////////

void pauseMenu(SDL_Renderer *renderer, SDL_Window *window, struct character *Player, int *done, int *pause){

    while (*pause){
        if (*done)
            break;
        
        checkEvents(window, Player, done, pause);
        drawScreen(renderer, pause);
        SDL_Delay(DELAY);

    }    
}

/////////////////////////////////////////////////////////////////
///                        MAIN                              ///
////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    SDL_Window *window;
    SDL_Renderer *renderer;

    struct character Player = {0, 0, 2, 1};

    int done = 0, pause = 0;
    int i, j;
    int pillcount = 0;
 
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Game Window",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_W, WINDOW_H, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    clearBoard();

//////////MAIN LOOP////////////
    while (!done){
        
        if (pause)
            pauseMenu(renderer, window, &Player, &done, &pause);

        checkEvents(window, &Player, &done, &pause);

        if (pillcount <= 0)
            pillcount = createPill(&tilelist);
        
        moveChar(&Player, &pillcount);

        drawScreen(renderer, &pause);

        SDL_Delay(DELAY);
}

    SDL_DestroyWindow(window);

    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}