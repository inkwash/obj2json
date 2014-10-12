#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv) {
	FILE *fp, *fp_out;
	float* unsorted_v = NULL;
	float* unsorted_vt = NULL;
	float* unsorted_vn = NULL;
	float* sorted_v = NULL;
	float* sorted_vt = NULL;
	float* sorted_vn = NULL;
	char line[1024];
	int count_v_lines = 0;
	int count_vt_lines = 0;
	int count_vn_lines = 0;
	int count_f_lines = 0;
	
	// validate args
	if (argc < 3) {
		printf ("Usage: ./convert input.obj output.json\n");
		return 0;
	}
	printf ("converting %s to %s\n", argv[1], argv[2]);
	
	// try open files
	fp = fopen (argv[1], "r");
	if (!fp) {
		fprintf (stderr, "ERROR: cannot open file %s\n", argv[1]);
		return 1;
	}
	fp_out = fopen (argv[2], "w");
	if (!fp_out) {
		fprintf (stderr, "ERROR: cannot open file %s\n", argv[2]);
		return 1;
	}
	
	// count stuff, rewind file
	line[0] = '\0';
	while (fgets (line, sizeof(line), fp)) {
		if (line[0] == 'v') {
			if (line[1] == 't') {
				count_vt_lines++;
			} else if (line[1] == 'n') {
				count_vn_lines++;
			} else if (line[1] == ' ') {
				count_v_lines++;
			}
		} else if ((line[0] == 'f') && (line[1] == ' ')) {
			count_f_lines++;
		}
	} // endwhile
	rewind (fp);
	
	// allocate
	unsorted_v = (float*)malloc (3 * count_v_lines * sizeof (float));
	unsorted_vt = (float*)malloc (2 * count_vt_lines * sizeof (float));
	unsorted_vn = (float*)malloc (3 * count_vn_lines * sizeof (float));
	sorted_v = (float*)malloc (9 * count_f_lines * sizeof (float));
	sorted_vt = (float*)malloc (6 * count_f_lines * sizeof (float));
	sorted_vn = (float*)malloc (9 * count_f_lines * sizeof (float));
	if (!unsorted_v || !unsorted_vt || !unsorted_vn || !sorted_v || !sorted_vt ||
		!sorted_vn) {
		fprintf (stderr, "malloc error\n");
		return 1;
	}
	
	// copy into malloc
	line[0] = '\0';
	count_v_lines = 0;
	count_vt_lines = 0;
	count_vn_lines = 0;
	count_f_lines = 0;
	while (fgets (line, sizeof(line), fp)) {
		if (line[0] == 'v') {
			if (line[1] == 't') {
				sscanf (line, "vt %f %f",
					&unsorted_vt[count_vt_lines * 2],
					&unsorted_vt[count_vt_lines * 2 + 1]
				);
				count_vt_lines++;
			} else if (line[1] == 'n') {
				sscanf (line, "vn %f %f %f",
					&unsorted_vn[count_vn_lines * 3],
					&unsorted_vn[count_vn_lines * 3 + 1],
					&unsorted_vn[count_vn_lines * 3 + 2]
				);
				count_vn_lines++;
			} else if (line[1] == ' ') {
				sscanf (line, "v %f %f %f",
					&unsorted_v[count_v_lines * 3],
					&unsorted_v[count_v_lines * 3 + 1],
					&unsorted_v[count_v_lines * 3 + 2]
				);
				count_v_lines++;
			}
		// assume all f lines are after others. sort the arrays
		} else if ((line[0] == 'f') && (line[1] == ' ')) {
			int tri_p_i[3], tri_vt_i[3], tri_vn_i[3], i;
			sscanf (line, "f %i/%i/%i %i/%i/%i %i/%i/%i",
				&tri_p_i[0], &tri_vt_i[0], &tri_vn_i[0],
				&tri_p_i[1], &tri_vt_i[1], &tri_vn_i[1],
				&tri_p_i[2], &tri_vt_i[2], &tri_vn_i[2]
			);
			for (i = 0; i < 3; i++) {
				sorted_v[count_f_lines * 9 + 0 + i * 3] =
					unsorted_v[(tri_p_i[i] - 1) * 3 + 0];
				sorted_v[count_f_lines * 9 + 1 + i * 3] =
					unsorted_v[(tri_p_i[i] - 1) * 3 + 1];
				sorted_v[count_f_lines * 9 + 2 + i * 3] =
					unsorted_v[(tri_p_i[i] - 1) * 3 + 2];
				sorted_vt[count_f_lines * 6 + 0 + i * 2] =
					unsorted_vt[(tri_vt_i[i] - 1) * 2 + 0];
				sorted_vt[count_f_lines * 6 + 1 + i * 2] =
					unsorted_vt[(tri_vt_i[i] - 1) * 2 + 1];
				sorted_vn[count_f_lines * 9 + 0 + i * 3] =
					unsorted_vn[(tri_vn_i[i] - 1) * 3 + 0];
				sorted_vn[count_f_lines * 9 + 1 + i * 3] =
					unsorted_vn[(tri_vn_i[i] - 1) * 3 + 1];
				sorted_vn[count_f_lines * 9 + 2 + i * 3] =
					unsorted_vn[(tri_vn_i[i] - 1) * 3 + 2];
			} // endfor
			count_f_lines++;
		} // endif
	} // endwhile

	printf ("writing json\n");
	{
		int i;
		fprintf (fp_out, "{\n");
		fprintf (fp_out, "\"vp\" : [");
		for (i = 0; i < count_f_lines * 9; i++) {
			fprintf (fp_out, "%.2f,", sorted_v[i]);
		}
		fprintf (fp_out, "],\n");
		
		fprintf (fp_out, "\"vt\" : [");
		for (i = 0; i < count_f_lines * 6; i++) {
			fprintf (fp_out, "%.2f,", sorted_vt[i]);
		}
		fprintf (fp_out, "],\n");
		
		fprintf (fp_out, "\"vn\" : [");
		for (i = 0; i < count_f_lines * 9; i++) {
			fprintf (fp_out, "%.2f,", sorted_vn[i]);
		}
		fprintf (fp_out, "]\n");
		fprintf (fp_out, "}\n");
	}
	fclose (fp);
	fclose (fp_out);
	
	free (unsorted_v);
	free (unsorted_vt);
	free (unsorted_vn);
	free (sorted_v);
	free (sorted_vt);
	free (sorted_vn);
	
	printf ("wrote %i points, memory freed\n", count_f_lines * 3);
	return 0;
}
