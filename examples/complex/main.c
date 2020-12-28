#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSONx.h"

struct date {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

enum booktype {
    BOOK_TECH = 0,
    BOOK_CARTOON
};

struct book {
    char* name;
    struct date time;
    enum booktype type;
    
    char isbn[11];

    char** authors;
    int author_cnt;

    short chapter_pages[15];
    int chapter_page_cnt;
};

const cjsonx_reflect_t date_reflection[] = {
    __cjsonx_int(struct date, year),
    __cjsonx_int(struct date, month),
    __cjsonx_int(struct date, day),
    __cjsonx_int(struct date, min),
    __cjsonx_int(struct date, hour),
    __cjsonx_int(struct date, sec),
    __cjsonx_end()
};

const cjsonx_reflect_t book_reflection[] = {
    __cjsonx_str_ptr(struct book, name),
    __cjsonx_object(struct book, time, date_reflection),
    __cjsonx_int(struct book, type),
    __cjsonx_str_ex(struct book, isbn, __serialized_name("ISBN")),
    __cjsonx_array_ptr_str_ptr(struct book, authors, author_cnt),
    __cjsonx_array_int(struct book, chapter_pages, chapter_page_cnt),
    __cjsonx_end()
};

int main(int argc, char* argv[]) {
    struct book b = {0};
    const char* json = "{\"name\":\"AAA\",\"ISBN\":\"1234567890\",\"type\":0,\"authors\""
        ":[\"a\",\"b\",\"c\"],\"time\":{\"year\":2020,\"month\":11,\"day\":10,\"hour\":12"
        ",\"min\":12,\"sec\":12}, \"chapter_pages\":[10,15,28,77]}";
    int ret = cjsonx_str2struct(json, &b, book_reflection);
    if (ret == ERR_CJSONX_NONE) {
        char* p;
        cjsonx_struct2str(&p, &b, book_reflection);
        printf("%s\n", p);
        cjsonx_reflection_ptr_free(&b, book_reflection);
        free(p);
    }
}

