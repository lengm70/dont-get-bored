#ifndef TWENTY_ONE_H
#define TWENTY_ONE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

enum { TWENTY_ONE_TARGET = 21, TWENTY_ONE_MIN_RANK = 1,
       TWENTY_ONE_MAX_RANK = 13, TWENTY_ONE_MAX_CARDS = 22,
       TWENTY_ONE_NAME_SIZE = 64 };
typedef enum { TWENTY_ONE_ACTIVE, TWENTY_ONE_WON,
               TWENTY_ONE_LOST, TWENTY_ONE_HELD } TwentyOneState;
typedef enum { TWENTY_ONE_SPADES, TWENTY_ONE_HEARTS,
               TWENTY_ONE_CLUBS, TWENTY_ONE_DIAMONDS } TwentyOneSuit;
typedef struct { int rank; TwentyOneSuit suit; } TwentyOneCard;
typedef struct {
    char name[TWENTY_ONE_NAME_SIZE];
    TwentyOneCard cards[TWENTY_ONE_MAX_CARDS];
    size_t card_count;
    int score;
    TwentyOneState state;
} TwentyOnePlayer;

/* Return 1 on success, 0 on invalid arguments/state; failures change nothing. */
int twenty_one_init(TwentyOnePlayer *player, const char *name);
/* A=1, J=11, Q=12, K=13. A scores 11 or 1; face cards score 10. */
int twenty_one_add_card(TwentyOnePlayer *player, int rank, TwentyOneSuit suit);
int twenty_one_hold(TwentyOnePlayer *player);
typedef enum { TWENTY_ONE_PENDING, TWENTY_ONE_PLAYER_WINS,
               TWENTY_ONE_AI_WINS, TWENTY_ONE_TIE } TwentyOneResult;
/* AI acts independently: draw below 17, hold at 17 or above. */
int twenty_one_ai_should_draw(const TwentyOnePlayer *ai);
TwentyOneResult twenty_one_result(const TwentyOnePlayer *player, const TwentyOnePlayer *ai);
#ifdef __cplusplus
}
#endif
#endif
