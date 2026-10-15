#include "TwentyOne.h"
#include <ctype.h>
#include <string.h>

int twenty_one_init(TwentyOnePlayer *player, const char *name) {
    const char *end;
    size_t length;
    if (!player || !name) return 0;
    while (*name && isspace((unsigned char)*name)) ++name;
    end = name + strlen(name);
    while (end > name && isspace((unsigned char)end[-1])) --end;
    length = (size_t)(end - name);
    if (!length || length >= TWENTY_ONE_NAME_SIZE) return 0;
    /* Local copy also permits restarting with player->name as the argument. */
    TwentyOnePlayer next = {0};
    memcpy(next.name, name, length);
    next.state = TWENTY_ONE_ACTIVE;
    *player = next;
    return 1;
}

int twenty_one_add_card(TwentyOnePlayer *player, int rank, TwentyOneSuit suit) {
    if (!player || player->state != TWENTY_ONE_ACTIVE ||
        rank < TWENTY_ONE_MIN_RANK || rank > TWENTY_ONE_MAX_RANK ||
        suit < TWENTY_ONE_SPADES || suit > TWENTY_ONE_DIAMONDS ||
        player->card_count >= TWENTY_ONE_MAX_CARDS) return 0;
    player->cards[player->card_count++] = (TwentyOneCard){rank, suit};
    /* Recalculate the best non-bust total, reducing aces one at a time. */
    int aces = 0;
    player->score = 0;
    for (size_t i = 0; i < player->card_count; ++i) {
        int r = player->cards[i].rank;
        if (r == 1) { player->score += 11; ++aces; }
        else player->score += r > 10 ? 10 : r;
    }
    while (player->score > TWENTY_ONE_TARGET && aces > 0) {
        player->score -= 10;
        --aces;
    }
    if (player->score == TWENTY_ONE_TARGET) player->state = TWENTY_ONE_WON;
    else if (player->score > TWENTY_ONE_TARGET) player->state = TWENTY_ONE_LOST;
    return 1;
}

int twenty_one_hold(TwentyOnePlayer *player) {
    if (!player || player->state != TWENTY_ONE_ACTIVE) return 0;
    player->state = TWENTY_ONE_HELD;
    return 1;
}

int twenty_one_ai_should_draw(const TwentyOnePlayer *ai) {
    return ai && ai->state == TWENTY_ONE_ACTIVE && ai->score < 17;
}

TwentyOneResult twenty_one_result(const TwentyOnePlayer *player, const TwentyOnePlayer *ai) {
    if (!player || !ai || player->state == TWENTY_ONE_ACTIVE || ai->state == TWENTY_ONE_ACTIVE)
        return TWENTY_ONE_PENDING;
    const int player_bust = player->score > TWENTY_ONE_TARGET;
    const int ai_bust = ai->score > TWENTY_ONE_TARGET;
    if (player_bust && ai_bust) return TWENTY_ONE_TIE;
    if (player_bust) return TWENTY_ONE_AI_WINS;
    if (ai_bust) return TWENTY_ONE_PLAYER_WINS;
    if (player->score == ai->score) return TWENTY_ONE_TIE;
    return player->score > ai->score ? TWENTY_ONE_PLAYER_WINS : TWENTY_ONE_AI_WINS;
}
