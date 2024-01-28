#include <string>
#include <utility>

#include "include/constants.h"
#include "include/game_nodes.h"
#include "include/text.h"
#include "include/time.h"

void CanvasNode::Render(CommandBuffer* buffer) {
    renderer_PushBase(buffer, v2(HALF_GAME_WIDTH, HALF_GAME_HEIGHT));
    Node::Render(buffer);
    renderer_PushBase(buffer, level->camera.center);
}

TextNode::TextNode(Level* level, Font* font, std::string str): Node(level), font(font), str(std::move(str)) {
}


void TextNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    renderer_PushString(buffer, font, str.c_str(),
                        GetAbsolutePosition(), DEPTH_UI);
}


void ScoreNode::Update() {
    TextNode::Update();

    str = string_format("Score: %d/%d", level->current_score, level->stage_attributes->score_requirement);

    if(level->current_score >= level->stage_attributes->max_score) {
        level->resetStage = true;
        level->ffGoNext = true;
    }
}


CountDownNode::CountDownNode(Level* level, Font* font)
    : TextNode(level, font, "Remaining: 0:00") {
    remaining_time = level->stage_attributes->time_limit;
}

void CountDownNode::Update() {
    TextNode::Update();


    remaining_time -= time_deltatime;

    if(remaining_time < 0) {
        remaining_time = 0;
        level->resetStage = true;
    }

    std::string prefix = "Remaining Time: ";
    const int time = static_cast <int> (std::ceil(remaining_time));;

    const auto seconds_total = time % 60;
    const auto minutes = (time - seconds_total) / 60;

    const auto second_back = seconds_total % 10;
    const auto second_front = (seconds_total - second_back) / 10;


    std::string timeStr = string_format("%d:%d%d", minutes, second_front, second_back);
    std::string total = prefix + timeStr;
    str = total.c_str();
}
