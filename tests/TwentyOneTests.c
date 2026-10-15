#include "games/twentyone/TwentyOne.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Failed line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#define ADD(r) twenty_one_add_card(&p,(r),TWENTY_ONE_HEARTS)
int main(void) {
    TwentyOnePlayer p;
    CHECK(!twenty_one_init(&p, NULL));
    CHECK(!twenty_one_init(&p, "  "));
    CHECK(twenty_one_init(&p, " Alice "));
    CHECK(strcmp(p.name, "Alice") == 0 && p.score == 0);
    CHECK(!ADD(0) && !ADD(14));
    CHECK(!twenty_one_add_card(&p,5,(TwentyOneSuit)4));
    CHECK(p.card_count == 0 && p.score == 0);
    CHECK(ADD(1) && p.score==11 && ADD(13));
    CHECK(p.score == 21 && p.state == TWENTY_ONE_WON);
    CHECK(p.cards[0].rank == 1 && p.cards[1].rank == 13);
    CHECK(p.cards[1].suit==TWENTY_ONE_HEARTS);
    CHECK(!ADD(2) && !twenty_one_hold(&p));
    CHECK(twenty_one_init(&p, p.name));
    CHECK(ADD(11) && ADD(12) && p.score==20 && ADD(2));
    CHECK(p.score == 22 && p.state == TWENTY_ONE_LOST);
    CHECK(!ADD(1));
    CHECK(twenty_one_init(&p, "Bob"));
    CHECK(ADD(8) && twenty_one_hold(&p));
    CHECK(p.score == 8 && p.state == TWENTY_ONE_HELD);
    CHECK(!ADD(1));
    CHECK(twenty_one_init(&p, "Longest hand"));
    for (int i = 0; i < 11; ++i) CHECK(ADD(1));
    CHECK(p.card_count == 11 && p.score==21 && p.state == TWENTY_ONE_WON);
    CHECK(!twenty_one_add_card(NULL, 1,TWENTY_ONE_SPADES) && !twenty_one_hold(NULL));
    CHECK(twenty_one_init(&p,"Soft ace")); CHECK(ADD(1) && ADD(9) && ADD(5));
    CHECK(p.score==15 && p.state==TWENTY_ONE_ACTIVE);
    CHECK(twenty_one_init(&p,"Two aces")); CHECK(ADD(1) && ADD(1) && p.score==12);
    CHECK(ADD(9) && p.score==21 && p.state==TWENTY_ONE_WON);
    CHECK(twenty_one_init(&p,"Low aces")); CHECK(ADD(1) && ADD(1) && ADD(1) && ADD(9));
    CHECK(p.score==12 && p.state==TWENTY_ONE_ACTIVE);
    for(int rank=2;rank<=13;++rank) {
        CHECK(twenty_one_init(&p,"Ranks")); CHECK(ADD(rank));
        CHECK(p.score==(rank>10?10:rank));
    }
    for(int suit=0;suit<4;++suit) {
        CHECK(twenty_one_init(&p,"Suits"));
        CHECK(twenty_one_add_card(&p,7,(TwentyOneSuit)suit));
        CHECK(p.score==7 && p.cards[0].suit==(TwentyOneSuit)suit);
    }
    TwentyOnePlayer ai;
    CHECK(twenty_one_init(&p,"Player") && twenty_one_init(&ai,"AI"));
    CHECK(ADD(8) && twenty_one_hold(&p));
    CHECK(twenty_one_add_card(&ai,6,TWENTY_ONE_SPADES));
    CHECK(twenty_one_result(&p,&ai)==TWENTY_ONE_PENDING);
    CHECK(twenty_one_ai_should_draw(&ai));
    CHECK(twenty_one_add_card(&ai,10,TWENTY_ONE_CLUBS));
    CHECK(twenty_one_ai_should_draw(&ai));
    CHECK(twenty_one_add_card(&ai,2,TWENTY_ONE_HEARTS));
    CHECK(!twenty_one_ai_should_draw(&ai) && twenty_one_hold(&ai));
    CHECK(twenty_one_result(&p,&ai)==TWENTY_ONE_AI_WINS);
    CHECK(p.score==8 && p.card_count==1);
    CHECK(twenty_one_init(&p,"Player") && twenty_one_init(&ai,"AI"));
    CHECK(ADD(1) && ADD(13));
    CHECK(twenty_one_result(&p,&ai)==TWENTY_ONE_PENDING);
    CHECK(twenty_one_add_card(&ai,1,TWENTY_ONE_SPADES));
    CHECK(twenty_one_add_card(&ai,10,TWENTY_ONE_HEARTS));
    CHECK(twenty_one_result(&p,&ai)==TWENTY_ONE_TIE);
    CHECK(!twenty_one_ai_should_draw(&ai));
    for (int humanScore=1;humanScore<=31;++humanScore) {
        for (int aiScore=1;aiScore<=31;++aiScore) {
            p.score=humanScore; p.state=TWENTY_ONE_HELD;
            ai.score=aiScore; ai.state=TWENTY_ONE_HELD;
            TwentyOneResult expected=TWENTY_ONE_TIE;
            if(humanScore>21 && aiScore<=21) expected=TWENTY_ONE_AI_WINS;
            else if(aiScore>21 && humanScore<=21) expected=TWENTY_ONE_PLAYER_WINS;
            else if(humanScore<=21 && aiScore<=21 && humanScore!=aiScore)
                expected=humanScore>aiScore?TWENTY_ONE_PLAYER_WINS:TWENTY_ONE_AI_WINS;
            CHECK(twenty_one_result(&p,&ai)==expected);
        }
    }
    ai.state=TWENTY_ONE_ACTIVE; ai.score=17;
    CHECK(!twenty_one_ai_should_draw(&ai));
    CHECK(twenty_one_result(&p,&ai)==TWENTY_ONE_PENDING);
    CHECK(twenty_one_result(NULL,&ai)==TWENTY_ONE_PENDING);
    puts("Poker rules, independent AI and 961 result combinations passed.");
    return 0;
}
