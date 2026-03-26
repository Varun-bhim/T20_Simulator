#include "simulator.h"

int main() {
    srand(time(NULL));

    // --- INNINGS 1 ---
    printf("================================================================================\n");
    printf("                          STARTING 1ST INNINGS (TEAM A)                         \n");
    printf("================================================================================\n");
    reset_innings_state();
    current_innings = 0;
    play_innings();
    
    team_scores[0] = global_score;
    team_wickets[0] = wickets_fallen;
    team_balls[0] = total_balls_bowled;
    target_score = global_score + 1;
    
    printf("\n>>> END OF 1ST INNINGS. TEAM B NEEDS %d RUNS TO WIN. <<<\n\n", target_score);
    usleep(2000000); 

    // --- INNINGS 2 ---
    printf("================================================================================\n");
    printf("                          STARTING 2ND INNINGS (TEAM B)                         \n");
    printf("================================================================================\n");
    reset_innings_state();
    current_innings = 1;
    play_innings();

    team_scores[1] = global_score;
    team_wickets[1] = wickets_fallen;
    team_balls[1] = total_balls_bowled;

    // --- FINAL MATCH SCORECARD & RESULT ---
    printf("\n\n################################################################################\n");
    printf("                               FULL MATCH SCORECARD                             \n");
    printf("################################################################################\n");
    
    print_team_innings(0, "TEAM A", "TEAM B");
    print_team_innings(1, "TEAM B", "TEAM A");

    printf("\n                            *** MATCH RESULT *** \n");
    if (team_scores[1] >= target_score) {
        printf("                       TEAM B WINS BY %d WICKETS!\n", 10 - team_wickets[1]);
    } else if (team_scores[1] == team_scores[0]) {
        printf("                       MATCH TIED! (Super Over needed!)\n");
    } else {
        printf("                       TEAM A WINS BY %d RUNS!\n", team_scores[0] - team_scores[1]);
    }
    printf("################################################################################\n\n");

    return 0;
}