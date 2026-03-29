#include "simulator.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "sjf") == 0) {
        use_sjf_scheduling = true; // SJF-mode, tail-ender priority
        printf("[Scheduler] SJF mode enabled: tail-enders get priority.\n");
    } else {
        use_sjf_scheduling = false; // normal with death over priority in last over
        printf("[Scheduler] FCFS mode enabled (with last-over death-over priority).\n");
    }

    srand(time(NULL));

    printf("================================================================================\n");
    printf("             2007 ICC T20 WORLD CUP FINAL: INDIA VS PAKISTAN                    \n");
    printf("================================================================================\n");
    
    // Perform the toss
    perform_toss();
    
    // Determine match flow based on toss
    int first_innings_team = batting_team;
    int second_innings_team = 1 - batting_team;
    batting_team_per_innings[0] = first_innings_team;
    batting_team_per_innings[1] = second_innings_team;
    
    is_second_innings = false;  // First innings is NOT a chase
    reset_innings_state();
    innings_index = 0;
    play_innings();
    
    team_scores[0] = global_score;
    team_wickets[0] = wickets_fallen;
    team_balls[0] = total_balls_bowled;
    innings_total_deliveries[0] = total_deliveries;  // Save total deliveries for innings 1
    target_score = global_score + 1;
    
    const char *first_team_name = (first_innings_team == 0) ? "INDIA" : "PAKISTAN";
    const char *second_team_name = (second_innings_team == 0) ? "INDIA" : "PAKISTAN";
    
    printf("\n>>> END OF INNINGS. %s NEEDS %d RUNS TO WIN THE WORLD CUP. <<<\n\n", second_team_name, target_score);
    usleep(2000000); 

    printf("================================================================================\n");
    printf("             STARTING 2ND INNINGS: %s CHASE BEGINS                        \n", second_team_name);
    printf("================================================================================\n\n");
    
    is_second_innings = true;  // NOW we're in the chasing innings
    reset_innings_state();
    innings_index = 1;
    play_innings();

    team_scores[1] = global_score;
    team_wickets[1] = wickets_fallen;
    team_balls[1] = total_balls_bowled;
    innings_total_deliveries[1] = total_deliveries;  // Save total deliveries for innings 2

    printf("\n\n################################################################################\n");
    printf("                        2007 WORLD CUP FINAL SCORECARD                          \n");
    printf("################################################################################\n");
    
    print_team_innings(0, first_team_name, second_team_name);
    print_team_innings(1, second_team_name, first_team_name);

    printf("\n                            *** MATCH RESULT *** \n");
    if (team_scores[1] >= target_score) {
        printf("                   %s WINS THE 2007 T20 WORLD CUP BY %d WICKETS!\n", second_team_name, 10 - team_wickets[1]);
    } else if (team_scores[1] == team_scores[0]) {
        printf("                   THE WORLD CUP FINAL IS TIED! BOWL OUT REQUIRED!\n");
    } else {
        printf("                   %s WINS THE 2007 T20 WORLD CUP BY %d RUNS!\n", first_team_name, team_scores[0] - team_scores[1]);
    }
    printf("################################################################################\n\n");

    // generate_gantt_chart();
    generate_striker_gantt_chart();
    generate_non_striker_gantt_chart();
    generate_bowler_gantt_chart();
    print_wait_time_analysis();

    return 0;
}
