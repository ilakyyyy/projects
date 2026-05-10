/*
    * This program extracts specific information from text files in a directory
    * and writes it to a CSV file.
    
    * Author: chatGPT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_LINE 256

// Helper function to extract a number after a specific label
void extract_value(const char *line, const char *label, char *output) {
    const char *ptr = strstr(line, label);
    if (ptr) {
        ptr += strlen(label);
        while (*ptr == ' ' || *ptr == ':') ptr++;
        strcpy(output, ptr);
        size_t len = strlen(output);
        if (len > 0 && output[len-1] == '\n') {
            output[len-1] = '\0';
        }
    }
}

int main() {
    DIR *d;
    struct dirent *dir;
    FILE *csv = fopen("results/summary.csv", "w");
    if (!csv) {
        perror("Cannot create summary.csv");
        return 1;
    }

    // Write CSV header
    fprintf(csv, "Student_NIP;Detection_shape;Questions_asked;Distance;Professors_left;Simulation_duration;Current_stress\n");

    printf("Extracting data from text files...\n");
    d = opendir("results");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt") && strlen(dir->d_name) > 4) {
                printf("Processing %s\n", dir->d_name);

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "results/%s", dir->d_name);

                FILE *txt = fopen(filepath, "r");
                if (!txt) {
                    perror("Cannot open txt file");
                    continue;
                }

                // Extract shape from filename: last number before .txt
                const char *dot = strrchr(dir->d_name, '.');
                const char *shape_char = (dot && dot > dir->d_name) ? dot - 1 : NULL;
                const char *shape_name = "UNKNOWN";
                if (shape_char) {
                    if (*shape_char == '0') shape_name = "CIRCLE";
                    else if (*shape_char == '1') shape_name = "CONE";
                }

                char line[MAX_LINE];
                char nip[32] = "", questions[32] = "", distance[32] = "";
                char professors_left[32] = "", duration[32] = "", stress[32] = "";

                while (fgets(line, sizeof(line), txt)) {
                    extract_value(line, "Student NIP", nip);
                    extract_value(line, "Questions asked", questions);
                    extract_value(line, "Distance", distance);
                    extract_value(line, "Professors left", professors_left);
                    extract_value(line, "Simulation duration", duration);
                    extract_value(line, "Current stress", stress);
                }

                fclose(txt);

                fprintf(csv, "%s;%s;%s;%s;%s;%s;%s\n",
                        nip, shape_name, questions, distance, professors_left, duration, stress);
            }
        }
        closedir(d);
    } else {
        perror("Cannot open results directory");
        fclose(csv);
        return 1;
    }

    fclose(csv);

    printf("Summary written to results/summary.csv\n");
    return 0;
}