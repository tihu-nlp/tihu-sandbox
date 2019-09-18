#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define name_of(x) #x
enum tihu_tag_t {
   N,
   N_PR,
   ADJ,
   PRO,
   ADV,
   P,
   V_PRS,
   CON,
   NUM,
   INT,
   ADJ_GEN
};

struct tihu_lexicon_entry_t {
    unsigned char *word;
    unsigned word_length;

    tihu_tag_t tag;
};

struct tihu_lexicon_entries_t {
    tihu_lexicon_entries_t() {
        size = 0;
        length = 0;
        entries = 0;
    }
    ~tihu_lexicon_entries_t() { free (entries); }

    void add(tihu_lexicon_entry_t entry) {
        if (length >= size) {
            size += (size >> 1) + 8;
            entries = (tihu_lexicon_entry_t *) realloc(entries, size * sizeof (tihu_lexicon_entry_t));
        }
        entries[length++] = entry;
    }

    unsigned size, length;
    tihu_lexicon_entry_t *entries;
};

struct tihu_lexicon_dictionary_t {
    tihu_lexicon_dictionary_t () {
        FILE *f = fopen("lexicon.dic", "r");
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        rewind(f);
        blob = (unsigned char *) malloc(length + 1);
        if (!blob) { fclose(f); return; }
        if (fread(blob, 1, length, f) != length) { free(blob); fclose(f); return; }
        blob[length] = 0;
        fclose(f);

        for (unsigned int i = 0; i < length; ++i) {
            unsigned char c = blob[i];
            unsigned word_length = 0;
            while (c != ' ' && c != '\n' && c != '/' && c != 0) {
                if (c >= 0xE0 && ++word_length == length - i) return;
                if (c >= 0xF0 && ++word_length == length - i) return;
                if (++word_length == length - i) return;
                c = blob[i + word_length];
            }
            if (!word_length) return;

            entries.add({ &blob[i], word_length, N });
            while (c != '\n') {
                if (c >= 0xE0 && ++i == length) return;
                if (c >= 0xF0 && ++i == length) return;
                if (++i == length) return;
                c = blob[i];
            }
        }
    }
    ~tihu_lexicon_dictionary_t () {
        free(blob);
    }

    tihu_lexicon_entries_t entries;

    private:
    unsigned char *blob;
    unsigned length;
};

int main() {
    tihu_lexicon_dictionary_t dic;
    printf ("%s\n", name_of (ADV));
    printf("%.*s\n", dic.entries.entries[0].word_length, dic.entries.entries[0].word);
}