#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* html_open =
"<!DOCTYPE html>"
"<html>"
"<head><meta charset='utf-8'>"
"<script>function h(s, t) {t.src = s;}</script>"
"</head>"
"<body>\n";

const char* html_close =
"</body></html>\n";

typedef struct Mapping {
	char* in;
	unsigned int uni;
} Mapping;

static const Mapping ru_map[] = {
	{ " ", 0x20 },
	{ "+", 0x301 },
	{ "m", 0x43c },{ "p", 0x43f },{ "b", 0x431 },
	{ "f", 0x444 },{ "v", 0x432 },

	{ "j_", 0x439 },{ "je", 0x435 },{ "ja", 0x44f },{ "jo", 0x451 },
	{ "ju", 0x44e },{ "js", 0x449 /*p.fric*/ },{ "j", 0x439 },

	{ "n", 0x43d },{ "d", 0x434 },
	{ "t_", 0x442 },{ "tjs", 0x447 /*t p.fric*/},
	{ "ts", 0x446 },{ "t", 0x442 },
	{ "s", 0x441 },{ "z", 0x437 },{ "l", 0x43b },{ "r", 0x440 },

	{ "hz", 0x436 },{ "hs", 0x448 }, // r.fric
	{ "k", 0x43a },{ "g", 0x433 },{ "x", 0x445 },
	{ "\"", 0x44a },{ "'", 0x44c },
	{ "i", 0x438 },{ "y", 0x44b },{ "e", 0x44d },{ "a", 0x430 },
		{ "o", 0x43e },{ "u", 0x443 },
};

void fpr_utf(FILE* fop, unsigned int uni) {
	if (uni < 0x80) { // single byte
		fprintf(fop, "%c", (uni & 255));
	} else if (uni < 0x800) { // two bytes
		fprintf(fop, "%c%c", 0xc0 | (uni >> 6), 0x80 | (uni & 0x3f));
	} else {
		fprintf(fop, "uwu nyaa~");
	}
}

void convert_ru(FILE* fop, char* line) {
	fprintf(fop, "<p>");
	for (;*line >= 0x20;) {
		for (int i = 0; i < sizeof(ru_map)/sizeof(Mapping); ++i) {
			int len = strlen(ru_map[i].in);
			int res = strncmp(ru_map[i].in, line, len);
			if (res == 0) {
				fpr_utf(fop, ru_map[i].uni);
				line += len;
				goto next;
			}
		}
		printf("Error on %s", line);
		return;
		next:;
	}
	fprintf(fop, "</p>\n");
}

void convert_fr(FILE* fop, char* line) {
	fprintf(fop, "<p></p>\n");
}

int convert_file(char* fin) {
	FILE* fip = fopen(fin, "r");
	size_t baselen = strlen(fin);
	char* fon = malloc(baselen + 2);
	strcpy(fon, fin);
	fon[baselen - 3] = 'h'; // assuming .txt file extension
	fon[baselen - 2] = 't';
	fon[baselen - 1] = 'm';
	fon[baselen    ] = 'l';
	fon[baselen + 1] = 0;
	FILE* fop = fopen(fon, "w");
	free(fon);
	fprintf(fop, html_open);
	char* line = NULL;
	size_t llen = 0;
	size_t nread = 0;
	while ((nread = getline(&line, &llen, fip)) != -1) {
		unsigned int code = 0;
		size_t i = 0;
		for (;; ++i) {
			unsigned char c = ((unsigned char*) line)[i];
			if (c <= 0x20) break;
			code <<= 8;
			code |= c;
		}
		++i; // get past the space
		size_t j = i;
		for (; line[j] != 10 && line[j] != 0; ++j);
		line[j] = 0;
		printf("%x %s\n", code, line);
		     if (code == 0x7275) convert_ru(fop, line+i);
		else if (code == 0x6672) convert_fr(fop, line+i);
		else if (code == 0x6c61)
			fprintf(fop, "<p>%s</p>", line+i);
		else if (code == 0x696d)
			fprintf(fop, "<img src='https://mincerafter42.github.io/assets/egs-8831.gif' onclick='this.src=\"%s\";'>\n", line+i);
		else if (code == 0) fprintf(fop, "<hr>");
	}
	free(line);
	fprintf(fop, html_close);
	fclose(fip);
	fclose(fop);
	return 0;
}

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		convert_file(argv[i]);
	}
	return 0;
}
