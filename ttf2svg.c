/*
ttf2svg.c
v3.0

ttf2svg is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 2 of the License, or (at your option) any later 
version.  ttf2svg is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  You can request a copy of the GNU General Public License from,
  Free Software Foundation, Inc.
  59 Temple Place, Suite 330, Boston, MA 02111-1307, USA

Copyright © 2012, 2013 xyzinfo <xyzinfo@gmail.com>

commandline:
  ttf2svg font.ttf codepoint
examples:
  ttf2svg kaiu.ttf 99AC > horse-traditional.svg
  ttf2svg simsun.ttc 9a6c > horse-simplified.svg
*/

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static int first_path;
static FT_Vector* first_point;

int is_first (const FT_Vector* to) {
	if (first_point) {
		if (to->x == first_point->x && to->y == first_point->y)
			return 1;
		else
			first_point = NULL;
	}
	return 0;
}

int move_to(const FT_Vector* to, void* user) {
	if (first_path) {
		printf("\n<path d=\"");
		first_path = 0;
	} else {
		printf("Z");
	}
	printf("M%ld %ld", to->x, to->y);
	first_point = to;
	return 0;
}

int line_to( const FT_Vector* to, void* user) {
	if (!is_first(to))
		printf("L%ld %ld", to->x, to->y);
	return 0;
}

int conic_to(const FT_Vector* ctrl, const FT_Vector* to, void* user) {
	if (!is_first(to))
		printf("Q%ld %ld %ld %ld", ctrl->x, ctrl->y, to->x, to->y);
	return 0;
}

int cubic_to(const FT_Vector* ctrl1, const FT_Vector* ctrl2,
             const FT_Vector *to, void* user) {
	if (!is_first(to))
		printf("\nC%ld %ld %ld %ld %ld %ld",
		       ctrl1->x, ctrl1->y, ctrl2->x, ctrl2->y, to->x, to->y);
	return 0;
}

void error_msg(FT_Error error, char *msg, int line) {
	fprintf(stderr, "FT_Error (%d) at line %d: %s\n", error, line, msg);
	exit(error);
}

int main(int argc, char *argv[]) {
	char *ttf, *str;
	FT_Error error;
	FT_Library library;
	FT_Face face;
	FT_ULong cp;
	int glyph_index;
	FT_Outline outline;
	FT_Outline_Funcs func_interface;
	FT_BBox bbox;

	if (argc == 3) {
		ttf = argv[1];
		str = argv[2];
	} else {
		fprintf(stderr, "Usage: %s font.ttf codepoint\n", argv[0]);
		exit(1);
	}
	cp = strtol(str, NULL, 16);
	if (cp == 0L) {
		fprintf(stderr, "Error: Invalid codepoint [%s]", str);
		exit(1);
	}

	error = FT_Init_FreeType(&library);
	if (error) error_msg(error, "FT_Init_FreeType", __LINE__);

	error = FT_New_Face(library, ttf, 0, &face);
	if (error) error_msg(error, "FT_New_Face", __LINE__);

	error = FT_Set_Pixel_Sizes(face, 0, face->units_per_EM / 64);
	if (error) error_msg(error, "FT_Set_Pixel_Sizes", __LINE__);

	glyph_index = FT_Get_Char_Index(face, cp);
	if (!glyph_index) error_msg(1, "FT_Get_Char_Index", __LINE__);

	FT_Int32 i = FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING;
	error = FT_Load_Glyph(face, glyph_index, i);
	if (error) error_msg(error, "FT_Load_Glyph", __LINE__);

	bbox = face->bbox;
	outline = face->glyph->outline;

	func_interface.shift = 0;
	func_interface.delta = 0;
	func_interface.move_to = move_to;
	func_interface.line_to = line_to;
	func_interface.conic_to = conic_to;
	func_interface.cubic_to = cubic_to;

	printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	printf("<svg xmlns=\"http://www.w3.org/2000/svg\"");
	printf(" viewBox=\"%ld %ld %ld %ld\">", bbox.xMin, bbox.yMin,
	       bbox.xMax - bbox.xMin, bbox.yMax - bbox.yMin);
	printf("\n<g name=\"&#x%x;\" transform=\"matrix(1 0 0 -1 0 %ld)\">",
	       cp, bbox.yMin + bbox.yMax);

	first_path = 1;
	first_point = NULL;
	error = FT_Outline_Decompose(&outline, &func_interface, NULL);
	if (error) error_msg(error, "FT_Outline_Decompose", __LINE__);

	printf("Z\"/>\n</g>\n</svg>\n");

	FT_Done_FreeType(library);
	return 0;
}
