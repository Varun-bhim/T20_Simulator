#include "simulator.h"

// 1. Check Target Function
int death_over_specialist_candidates[4] = {11, 10, 9, 8}; // tail-enders / specialists for last over

void perform_toss() {
    toss_winner = rand() % 2;  // 0 = India, 1 = Pakistan
    toss_choice = rand() % 2;  // 0 = Bat, 1 = Bowl
    
    const char *teams[2] = {"INDIA", "PAKISTAN"};
    const char *choices[2] = {"BAT", "BOWL"};
    
    printf("\n================================================================================\n");
    printf("                           TOSS CEREMONY                                      \n");
    printf("================================================================================\n");
    printf("%s WON THE TOSS AND ELECTED TO %s FIRST\n", teams[toss_winner], choices[toss_choice]);
    printf("================================================================================\n\n");
    
    // Determine who bats first
    if (toss_choice == 0) {  // Toss winner chose to bat
        batting_team = toss_winner;
    } else {  // Toss winner chose to bowl
        batting_team = 1 - toss_winner;
    }
    
    usleep(1000000);  // Brief pause for drama
}

int pick_next_batsman() {
    // Last-over death-over priority (high match intensity)
    // If 6 legal balls left or fewer, activate death-over priority
    if ((120 - total_balls_bowled) <= 6) {
        for (int i = 0; i < 4; i++) {
            int specialist = death_over_specialist_candidates[i];
            if (!batter_stats[current_innings][specialist].has_batted && specialist != striker_id && specialist != non_striker_id) {
                printf("[Scheduler] DEATH OVER priority: selecting specialist %s\n", get_bat_name(current_innings, specialist));
                return specialist;
            }
        }
    }

    while (next_batsman_id <= 11) {
        int candidate = spawn_order[next_batsman_id];
        next_batsman_id++;
        if (candidate >= 1 && candidate <= 11 && !batter_stats[current_innings][candidate].has_batted) {
            return candidate;
        }
    }
    return -1;
}

void check_target() {
    // Only check target in second innings (chasing team) and only if target_score is valid
    if (is_second_innings && target_score > 0 && global_score >= target_score) {
        printf("\n>>> TARGET CHASED DOWN! %s WINS! <<<\n", (current_innings == 0) ? "INDIA" : "PAKISTAN");
        match_over = true;
        pthread_cond_broadcast(&next_ball_cond);
        pthread_cond_broadcast(&ball_bowled_cond);
        pthread_cond_broadcast(&ball_in_air_cond);
    }
}

// 2. Reset State Function (With OS Clock Reset)
void reset_innings_state() {
    global_score = 0;
    total_balls_bowled = 0;
    total_deliveries = 0;  // Reset all deliveries counter
    system_ticks = 0; // OS Clock reset
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
    next_batsman_id = 3;  // RESET THIS - was missing!
    active_bowler_id = 1;
}

// 3. Play Innings Function
void play_innings() {
    sem_init(&crease_sem, 0, 2); 
    
    pthread_t fielders[10];
    int fielder_ids[10];
    for (int i = 0; i < 10; i++) {
        fielder_ids[i] = i + 1;
        pthread_create(&fielders[i], NULL, fielder_action, &fielder_ids[i]);
    }

    if (use_sjf_scheduling) {
        int sjf_order[] = {0, 1, 2, 8, 9, 10, 11, 3, 4, 5, 6, 7}; 
        for(int i=1; i<=11; i++) spawn_order[i] = sjf_order[i];
    } else {
        int fcfs_order[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; 
        for(int i=1; i<=11; i++) spawn_order[i] = fcfs_order[i];
    }

    for (int i = 1; i <= 2; i++) { 
        int id = spawn_order[i];
        bat_ids[id] = id;
        pthread_create(&batsmen[id], NULL, batsman_action, &bat_ids[id]);
    }
    next_batsman_id = 3; 

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
    for (int i = 1; i <= 11; i++) {
        if (batter_stats[current_innings][i].has_batted) pthread_join(batsmen[i], NULL);
    }

    sem_destroy(&crease_sem);
}

// 4. Print Team Innings Function
void print_team_innings(int inn_idx, const char* bat_team, const char* bowl_team) {
    int total_wides = 0; int total_no_balls = 0;
    for (int i = 1; i <= 5; i++) {
        total_wides += bowler_stats[inn_idx][i].wides;
        total_no_balls += bowler_stats[inn_idx][i].no_balls;
    }

    printf("\n%s INNINGS: %d / %d (Overs: %d.%d) | Extras: %d\n", 
           bat_team, team_scores[inn_idx], team_wickets[inn_idx], team_balls[inn_idx] / 6, team_balls[inn_idx] % 6, total_wides + total_no_balls);
    printf("--------------------------------------------------------------------------------\n");
    printf("%-15s %-6s %-6s %-5s %-5s %-8s %-12s\n", "Batter", "Runs", "Balls", "4s", "6s", "SR", "Status");
    for (int i = 1; i <= 11; i++) {
        if (batter_stats[inn_idx][i].has_batted) {
            float sr = batter_stats[inn_idx][i].balls > 0 ? ((float)batter_stats[inn_idx][i].runs / batter_stats[inn_idx][i].balls) * 100 : 0.0;
            printf("%-15s %-6d %-6d %-5d %-5d %-8.1f %-12s\n", 
                   get_bat_name(inn_idx, i), batter_stats[inn_idx][i].runs, batter_stats[inn_idx][i].balls, batter_stats[inn_idx][i].fours, batter_stats[inn_idx][i].sixes, sr, batter_stats[inn_idx][i].is_out ? "Out" : "Not Out");
        }
    }
    printf("\n%-15s %-10s %-10s %-10s %-10s %-5s %-5s\n", "Bowler", "Overs", "Runs", "Wickets", "Econ", "Wd", "NB");
    for (int i = 1; i <= 5; i++) {
        if (bowler_stats[inn_idx][i].balls_bowled > 0 || bowler_stats[inn_idx][i].runs_conceded > 0) {
            float overs = bowler_stats[inn_idx][i].balls_bowled / 6.0;
            float econ = overs > 0 ? bowler_stats[inn_idx][i].runs_conceded / overs : 0.0;
            printf("%-15s %-10.1f %-10d %-10d %-10.1f %-5d %-5d\n", 
                    get_bowl_name(inn_idx, i), overs, bowler_stats[inn_idx][i].runs_conceded, bowler_stats[inn_idx][i].wickets, econ, bowler_stats[inn_idx][i].wides, bowler_stats[inn_idx][i].no_balls);
        }
    }
    printf("================================================================================\n");
}

// 5. Generate Gantt Chart Function (With OS Clock Fix)
void generate_gantt_chart() {
    FILE *fp = fopen("gantt.md", "w");
    if (fp == NULL) return;

    fprintf(fp, "```mermaid\n");
    fprintf(fp, "gantt\n");
    fprintf(fp, "    title Pitch Resource Allocation (OS CPU Ticks)\n");
    fprintf(fp, "    dateFormat  X\n");
    fprintf(fp, "    axisFormat %%s\n\n");

    fprintf(fp, "    section Innings 1 (India)\n");
    for (int i = 1; i <= 11; i++) {
        if (batter_stats[0][i].has_batted) {
            int start = start_time[0][i];
            int duration = end_time[0][i] - start_time[0][i];
            if (duration <= 0) duration = 1; 
            fprintf(fp, "    %s : %d, %dd\n", get_bat_name(0, i), start, duration);
        }
    }

    fprintf(fp, "    section Innings 2 (Pakistan)\n");
    for (int i = 1; i <= 11; i++) {
        if (batter_stats[1][i].has_batted) {
            int start = start_time[1][i];
            int duration = end_time[1][i] - start_time[1][i];
            if (duration <= 0) duration = 1; 
            fprintf(fp, "    %s : %d, %dd\n", get_bat_name(1, i), start, duration);
        }
    }
    
    fprintf(fp, "```\n");
    fclose(fp);
    printf("\n>>> DELIVERABLE 2: Gantt chart perfectly generated in 'gantt.md' <<<\n");
}

// 6. Generate Striker Gantt Chart Function
void generate_striker_gantt_chart() {
    FILE *fp = fopen("striker_gantt.md", "w");
    if (fp == NULL) return;

    fprintf(fp, "```mermaid\n");
    fprintf(fp, "gantt\n");
    fprintf(fp, "    title Striker Batsman at Crease (Delivery Ranges - Including Wides/No-balls)\n");
    fprintf(fp, "    dateFormat X\n");
    fprintf(fp, "    axisFormat %%d\n\n");

    fprintf(fp, "    section Innings 1 (India)\n");
    for (int inn = 0; inn < 2; inn++) {
        if (inn == 1) fprintf(fp, "    section Innings 2 (Pakistan)\n");
        
        int max_deliveries = innings_total_deliveries[inn];
        int current_striker = -1;
        int start_delivery = 1;

        for (int delivery = 1; delivery <= max_deliveries; delivery++) {
            int striker = striker_per_ball[inn][delivery];
            
            if (striker != current_striker) {
                if (current_striker != -1) {
                    // Output the previous range
                    int end_delivery = delivery - 1;
                    fprintf(fp, "    %s : %d, %d\n", get_bat_name(inn, current_striker), start_delivery, end_delivery - start_delivery + 1);
                }
                current_striker = striker;
                start_delivery = delivery;
            }
        }
        
        // Output the last range
        if (current_striker != -1) {
            fprintf(fp, "    %s : %d, %d\n", get_bat_name(inn, current_striker), start_delivery, max_deliveries - start_delivery + 1);
        }
    }
    
    fprintf(fp, "```\n");
    fclose(fp);
    printf("\n>>> Striker Gantt chart generated in 'striker_gantt.md' <<<\n");
}

// 7. Generate Non-Striker Gantt Chart Function
void generate_non_striker_gantt_chart() {
    FILE *fp = fopen("non_striker_gantt.md", "w");
    if (fp == NULL) return;

    fprintf(fp, "```mermaid\n");
    fprintf(fp, "gantt\n");
    fprintf(fp, "    title Non-Striker Batsman at Crease (Delivery Ranges - Including Wides/No-balls)\n");
    fprintf(fp, "    dateFormat X\n");
    fprintf(fp, "    axisFormat %%d\n\n");

    fprintf(fp, "    section Innings 1 (India)\n");
    for (int inn = 0; inn < 2; inn++) {
        if (inn == 1) fprintf(fp, "    section Innings 2 (Pakistan)\n");
        
        int max_deliveries = innings_total_deliveries[inn];
        int current_non_striker = -1;
        int start_delivery = 1;

        for (int delivery = 1; delivery <= max_deliveries; delivery++) {
            int non_striker = non_striker_per_ball[inn][delivery];
            
            if (non_striker != current_non_striker) {
                if (current_non_striker != -1) {
                    // Output the previous range
                    int end_delivery = delivery - 1;
                    fprintf(fp, "    %s : %d, %d\n", get_bat_name(inn, current_non_striker), start_delivery, end_delivery - start_delivery + 1);
                }
                current_non_striker = non_striker;
                start_delivery = delivery;
            }
        }
        
        // Output the last range
        if (current_non_striker != -1) {
            fprintf(fp, "    %s : %d, %d\n", get_bat_name(inn, current_non_striker), start_delivery, max_deliveries - start_delivery + 1);
        }
    }
    
    fprintf(fp, "```\n");
    fclose(fp);
    printf("\n>>> Non-Striker Gantt chart generated in 'non_striker_gantt.md' <<<\n");
}

// 8. Generate Bowler Gantt Chart Function
void generate_bowler_gantt_chart() {
    FILE *fp = fopen("bowler_gantt.md", "w");
    if (fp == NULL) return;

    fprintf(fp, "```mermaid\n");
    fprintf(fp, "gantt\n");
    fprintf(fp, "    title Bowler at Crease (Delivery Ranges - Including Wides/No-balls)\n");
    fprintf(fp, "    dateFormat X\n");
    fprintf(fp, "    axisFormat %%d\n\n");

    fprintf(fp, "    section Innings 1 (India Bowlers)\n");
    for (int inn = 0; inn < 2; inn++) {
        if (inn == 1) fprintf(fp, "    section Innings 2 (Pakistan Bowlers)\n");
        
        int max_deliveries = innings_total_deliveries[inn];
        int current_bowler = -1;
        int start_delivery = 1;

        for (int delivery = 1; delivery <= max_deliveries; delivery++) {
            int bowler = bowler_per_ball[inn][delivery];
            
            if (bowler != current_bowler) {
                if (current_bowler != -1) {
                    // Output the previous range
                    int end_delivery = delivery - 1;
                    fprintf(fp, "    %s : %d, %d\n", get_bowl_name(inn, current_bowler), start_delivery, end_delivery - start_delivery + 1);
                }
                current_bowler = bowler;
                start_delivery = delivery;
            }
        }
        
        // Output the last range
        if (current_bowler != -1) {
            fprintf(fp, "    %s : %d, %d\n", get_bowl_name(inn, current_bowler), start_delivery, max_deliveries - start_delivery + 1);
        }
    }
    
    fprintf(fp, "```\n");
    fclose(fp);
    printf("\n>>> Bowler Gantt chart generated in 'bowler_gantt.md' <<<\n");
}

// 9. Print Wait Time Analysis Function
void print_wait_time_analysis() {
    printf("\n################################################################################\n");
    printf("         DELIVERABLE 3: SCHEDULING ANALYSIS (FCFS vs. SJF)                      \n");
    printf("################################################################################\n\n");

    printf("1. The FCFS Reality (First-Come, First-Serve):\n");
    printf("   - In real T20 cricket, the batting order follows an FCFS scheduler.\n");
    printf("   - OS Concept: Top-order batsmen act as 'Long Jobs' with high CPU burst times.\n");
    printf("   - Result: This causes the 'Convoy Effect'. Middle-order threads must wait \n");
    printf("     in the Ready Queue (Pavilion) for an extended period, leading to very high \n");
    printf("     Average Wait Times.\n\n");

    printf("2. The SJF Optimization (Shortest Job First):\n");
    printf("   - If we optimized this strictly as an Operating System, we would use SJF.\n");
    printf("   - OS Concept: We would dispatch Tail-enders (Bowlers) first because they \n");
    printf("     have the shortest expected CPU burst times (they get out quickly).\n");
    printf("   - Result: This mathematically minimizes the Average Wait Time for the system \n");
    printf("     and completely eliminates the Convoy Effect.\n\n");

    printf("3. Final Conclusion (OS Efficiency vs. Domain Constraints):\n");
    printf("   - From a pure OS perspective, SJF is the vastly superior scheduling algorithm.\n");
    printf("   - However, applying SJF violates the Domain Constraints of cricket. Sending \n");
    printf("     tail-enders to face the new ball would cause a rapid system collapse.\n");
    printf("   - Therefore, the simulator sacrifices OS CPU efficiency (accepting high wait \n");
    printf("     times) to satisfy the application's ultimate goal: scoring runs.\n");
    printf("################################################################################\n\n");
}
