#pragma once
#include "raylib.h"
namespace games::twentyone::screenConfig {
inline constexpr Rectangle panel{18,16,924,568};
inline constexpr float titleY=27, titleSize=30, summaryY=75, potY=103;
inline constexpr float tableX=480,tableY=260,tableRadiusX=278,tableRadiusY=117;
inline constexpr float orbitRadiusX=312,orbitRadiusY=110;
inline constexpr Rectangle frontHand{200,318,560,148};
inline constexpr float distantWidth=196,distantHeight=88,dealDuration=.30f;
inline constexpr int tableSegments=64;
inline constexpr float seatCut=12, nameInset=14, nameSize=16, detailY=34, detailSize=12, handY=60, handHeight=88;
inline constexpr int handColumns=11;
inline constexpr float maxCardWidth=60, cardGap=4, cardRatio=1.4f;
inline constexpr Rectangle setupFields[3]{{380,154,240,40},{380,214,240,40},{380,274,240,40}};
inline constexpr float setupLabelX=220, setupLabelOffset=9, setupSeatX=90, setupSeatStep=200;
inline constexpr float setupSeatY=353, setupSeatWidth=180;
inline constexpr Rectangle startButton{380,478,200,40};
inline constexpr Rectangle drawButton{65,502,145,48}, raiseButton{225,502,205,48}, holdButton{445,502,145,48};
inline constexpr Rectangle raiseField{650,500,235,50}, nextButton{370,502,220,48};
inline constexpr float raiseLabelX=650, raiseLabelY=475, raiseLabelSize=18, raiseValueSize=28;
inline constexpr float scoreBadgeWidth=92, scoreBadgeHeight=48, scoreBadgeInset=8, scoreSize=28;
inline constexpr Rectangle winnerPanel{170,158,620,296}, winnerButton{370,390,220,48};
inline constexpr float winnerTitleY=183, winnerNamesY=236, winnerFundsY=303;
inline constexpr float winnerTitleSize=30, winnerNamesSize=28, winnerFundsSize=36, winnerPadding=40;
inline constexpr Rectangle backButton{30,30,100,34};
inline constexpr float resultY=476, footerY=557, hintSize=12;
}  // namespace games::twentyone::screenConfig
