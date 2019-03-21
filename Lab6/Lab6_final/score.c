/*
 * score.c
 *
 *  Created on: Mar 14, 2019
 *      Author: William
 */
#include "score.h"

/***** Global Variables *****/
ScoreEntry scores[NUM_SCORES_MAX];

/***** Internal ****/

/***** API *****/
int score_load_from_get_response(char *response_body)
{
    long lRetVal = 0;
    int i = 0;
    jsonParser  jParser;
    jsonArray   jArray;
    jsonObj parent_j;
    jsonObj member_j;

    parent_j = json_parser_init(&jParser, response_body);
    memset(scores,0,(sizeof(ScoreEntry) * NUM_SCORES_MAX));

    // Print the Json Parser Data members
    UART_PRINT("\r\nJson Parser Data Members\r\n");
    UART_PRINT("JsonStream:\r\n%s\r\nnoOfToken:\r\n%d\r\n\r\n",jParser.jsonStream, jParser.noOfToken);

    member_j = json_get_object(&jParser, parent_j, "state");
    member_j = json_get_object(&jParser, member_j, "desired");
    jArray   = json_object_get_array(&jParser,member_j,"HighScores");

    member_j = json_array_get_object(&jParser,jArray,i);
    while(member_j != -1) {
        if (NULL != json_object_get_string(&jParser, member_j, "name", scores[i].name)) {
            scores[i].score = json_object_get_int(&jParser, member_j, "score");
            UART_PRINT("name: %s, score %d\r\n", scores[i].name, scores[i].score);
        }

        member_j = json_array_get_object(&jParser,jArray,++i);
    }

    json_parser_deinit(&jParser);
    return lRetVal;
}

/* Creates a JSON formatted object with all the high score data */
int score_save_to_post_request(char *request_body) {

    int i = 0;
    char tmp[64];

    // Write the header
    strcpy(request_body,POST_SCORE_HEAD);

    // Fill the formated data part
    for (i = 0; i < NUM_SCORES_MAX; i++) {
        sprintf(tmp,"%d. %s %d\\r\\n", i+1, scores[i].name, scores[i].score);
        strcat(request_body,tmp);
    }

    // End formatted part and start array
    strcat(request_body, POST_SCORE_FORMATTED_END);
    strcat(request_body, POST_SCORE_ARRAY_START);

    // Fill out array of high scores
    for (i = 0; i < NUM_SCORES_MAX; i++) {
        sprintf(tmp, POST_SCORE_NAME "%s" POST_SCORE_SCORE "%d", scores[i].name, scores[i].score);
        strcat(request_body,tmp);
        if (i < NUM_SCORES_MAX - 1) {
            strcat(request_body,POST_SCORE_END);
        }
    }

    // Write the tail
    strcat(request_body,POST_SCORE_TAIL);
    return 0;
}

int score_add(char *name, int score)
{
    int i = 0;
    int new_placement = 0;
    ScoreEntry oldEntry;
    ScoreEntry tmpEntry;

    for (i = 0; i < NUM_SCORES_MAX; i++) {
        if (score > scores[i].score && !new_placement) {
            oldEntry = scores[i];
            strcpy(scores[i].name, name);
            scores[i].score = score;
            new_placement = i+1;
        } else if (new_placement) {
            tmpEntry = scores[i];
            scores[i] = oldEntry;
            oldEntry = tmpEntry;
        }
    }

    return new_placement;
}

int score_get(char *name, int *score, int rank) {

    if (rank > NUM_SCORES_MAX || rank < 1) {
        return -1;
    }

    strcpy(name,scores[rank-1].name);
    *score = scores[rank-1].score;
    return 0;
}

void score_clear(void) {
    memset(scores,0,(sizeof(ScoreEntry) * NUM_SCORES_MAX));
}
