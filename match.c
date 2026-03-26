#include "simulator.h"

void check_target() {
    if (current_innings == 1 && global_score >= target_score) {
        printf("\n>>> TARGET CHASED DOWN! <<<\n");
        match_over = true;
        pthread_cond_broadcast(&next_ball_cond);
        pthread_cond_broadcast(&ball_bowled_cond);
        pthread_cond_broadcast(&ball_in_air_cond);
    }
}

void reset_innings_state() {
    global_score = 0;
    total_balls_bowled = 0;
    wickets_fallen = 0;
    match_over = false;
    ball_ready = false; 
    ball_in_air = false;
    current_is_wide = false;
    current_is_no_ball = false;
    active_free_hit = false;
    run_out_in_progress = false;

    striker_id = 1;
    non_striker_id = 2;
    next_batsman_id = 3; 
    active_bowler_id = 1;
}

void play_innings() {
    sem_init(&crease_sem, 0, 2); 
    
    pthread_t fielders[10];
    int fielder_ids[10];
    for (int i = 0; i < 10; i++) {
        fielder_ids[i] = i + 1;
        pthread_create(&fielders[i], NULL, fielder_action, &fielder_ids[i]);
    }

    for (int i = 1; i <= 2; i++) { 
        bat_ids[i] = i;
        pthread_create(&batsmen[i], NULL, batsman_action, &bat_ids[i]);
    }

    pthread_t bowler;
    int current_bowler = 1;
    
    while (total_balls_bowled < 120 && wickets_fallen < 10 && !match_over) {
        pthread_create(&bowler, NULL, bowler_action, &current_bowler);
        pthread_join(bowler, NULL); 
        current_bowler = (current_bowler % 5) + 1; 
    }

    match_over = true;
    ball_ready = true; 
    pthread_cond_broadcast(&ball_bowled_cond);
    pthread_cond_broadcast(&ball_in_air_cond); 
    pthread_cond_broadcast(&next_ball_cond);

    for (int i = 0; i < 10; i++) pthread_join(fielders[i], NULL);
    for (int i = 1; i < next_batsman_id; i++) pthread_join(batsmen[i], NULL);

    sem_destroy(&crease_sem);
}

void print_team_innings(int inn_idx, const char* bat_team, const char* bowl_team) {
    int total_wides = 0; int total_no_balls = 0;
    for (int i = 1; i <= 5; i++) {
        total_wides += bowler_stats[inn_idx][i].wides;
        total_no_balls += bowler_stats[inn_idx][i].no_balls;
    }

    printf("\n%s INNINGS: %d / %d (Overs: %d.%d) | Extras: %d\n", 
           bat_team, team_scores[inn_idx], team_wickets[inn_idx], team_balls[inn_idx] / 6, team_balls[inn_idx] % 6, total_wides + total_no_balls);
    printf("--------------------------------------------------------------------------------\n");
    printf("%-8s %-6s %-6s %-5s %-5s %-8s %-12s\n", "Batter", "Runs", "Balls", "4s", "6s", "SR", "Status");
    for (int i = 1; i <= 11; i++) {
        if (batter_stats[inn_idx][i].has_batted) {
            float sr = batter_stats[inn_idx][i].balls > 0 ? ((float)batter_stats[inn_idx][i].runs / batter_stats[inn_idx][i].balls) * 100 : 0.0;
            printf("Bat %-4d %-6d %-6d %-5d %-5d %-8.1f %-12s\n", 
                   i, batter_stats[inn_idx][i].runs, batter_stats[inn_idx][i].balls, batter_stats[inn_idx][i].fours, batter_stats[inn_idx][i].sixes, sr, batter_stats[inn_idx][i].is_out ? "Out" : "Not Out");
        }
    }
    printf("\n%-10s %-10s %-10s %-10s %-10s %-5s %-5s\n", "Bowler", "Overs", "Runs", "Wickets", "Econ", "Wd", "NB");
    for (int i = 1; i <= 5; i++) {
        if (bowler_stats[inn_idx][i].balls_bowled > 0 || bowler_stats[inn_idx][i].runs_conceded > 0) {
            float overs = bowler_stats[inn_idx][i].balls_bowled / 6.0;
            float econ = overs > 0 ? bowler_stats[inn_idx][i].runs_conceded / overs : 0.0;
            printf("Bowl %-5d %-10.1f %-10d %-10d %-10.1f %-5d %-5d\n", 
                    i, overs, bowler_stats[inn_idx][i].runs_conceded, bowler_stats[inn_idx][i].wickets, econ, bowler_stats[inn_idx][i].wides, bowler_stats[inn_idx][i].no_balls);
        }
    }
    printf("================================================================================\n");
}