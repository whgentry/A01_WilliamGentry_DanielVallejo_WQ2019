/*
 * score.h
 *
 *  Created on: Mar 14, 2019
 *      Author: William
 */

#ifndef SCORE_H_
#define SCORE_H_

#include "project.h"

/***** Macros *****/
#define NUM_SCORES_MAX 10

#define POST_SCORE_HEAD             "{\"state\": {\r\n\"desired\" : {\r\n\"var\" : \"High Scores!\\r\\n"
#define POST_SCORE_EX_DATA          "Bill: 10\",\r\n" \
                                        "\"HighScores\" : [\r\n" \
                                           "{\"name\":\"Willy\", \"score\":10},\r\n" \
                                           "{\"name\":\"Katie\", \"score\":9}\r\n"
#define POST_SCORE_FORMATTED_END    "\",\r\n"
#define POST_SCORE_ARRAY_START      "\"HighScores\" : [\r\n"
#define POST_SCORE_NAME             "{\"name\":\""
#define POST_SCORE_SCORE            "\", \"score\":"
#define POST_SCORE_END              "},\r\n"
#define POST_SCORE_TAIL             "}\r\n]\r\n}}}\r\n\r\n"


/***** Structs *****/
typedef struct ScoreEntry {
    char name[32];
    int  score;
} ScoreEntry;


/***** API ****/
int score_load_from_get_response(char *response_body);
int score_save_to_post_request(char *request_body);
int score_add(char *name, int score);
int score_get(char *name, int *score, int rank);
void score_clear(void);


#endif /* SCORE_H_ */
