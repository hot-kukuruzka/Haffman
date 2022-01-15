#include <bits/stdc++.h>

using namespace std;
typedef long long ll;

struct Forest {
    ll freq;
    int root;
};

struct Tree {
    int left = -1, right = -1, parent = -1, symbol;
};

struct Code {
    int len;
    int code[256];
};

Forest forest[256];
Tree tree[520];
Code coding[256];
FILE *f1, *f2;

void min_min(Forest forest[], int _size, int &pos1, int &pos2) {
    if (forest[0].freq < forest[1].freq) {
        pos1 = 0;
        pos2 = 1;
    } else {
        pos1 = 1;
        pos2 = 0;
    }
    for (int i = 2; i < _size; i++) {
        if (forest[i].freq < forest[pos1].freq) {
            pos2 = pos1;
            pos1 = i;
        }
        else {
            if (forest[i].freq < forest[pos2].freq)
                pos2 = i;
        }
    }
}

int32_t main(int argc, char *argv[]) {
    if (!strcmp("encode", argv[1])) {
        f1 = fopen(argv[3], "rb");
        f2 = fopen(argv[2], "wb");
        ll freq_t[256];
        ll fr[256];
        int root_t = 0, numbers = 0, j = 0, i;

        memset(freq_t, 0, sizeof(freq_t));

        unsigned char ch;
        while (fscanf(f1, "%c", &ch) != -1) {
            freq_t[ch]++;
        }
        for (i = 0; i < 256; i++) {
            if (freq_t[i] != 0) {
                fr[j] = freq_t[i];
                j++;
                forest[root_t].freq = freq_t[i];
                forest[root_t].root = root_t;
                tree[root_t].symbol = i;
                root_t++;
            }
        }
        int sizeforest = root_t, cnt_of_num = root_t;
        int sizetree = sizeforest;
        int p1, p2;
        while (sizeforest > 1) {
            min_min(forest, sizeforest, p1, p2);

            tree[sizetree].left = forest[p1].root;
            tree[sizetree].right = forest[p2].root;
            tree[sizetree].parent = -1;

            tree[forest[p1].root].parent = sizetree;
            tree[forest[p2].root].parent = sizetree;

            forest[p1].freq += forest[p2].freq;
            forest[p1].root = sizetree;

            forest[p2] = forest[sizeforest - 1];

            sizeforest--;
            sizetree++;
        }
        fprintf(f2, "%d\n", sizetree);
        for (i = 0; i < sizetree; i++) fprintf(f2, "%d %d %d %d\n",tree[i].left, tree[i].right, tree[i].parent, tree[i].symbol);
        int all_bytes = 0, cnt = 0;
        for (i = 0; i < cnt_of_num; i++) {
            int cur_index = i;
            while (tree[cur_index].parent != -1) {
                int prev_index = cur_index;
                cur_index = tree[cur_index].parent;
                if (tree[cur_index].left == prev_index) {
                    coding[tree[i].symbol].code[coding[tree[i].symbol].len] = 0;
                    coding[tree[i].symbol].len++;
                } else if (tree[cur_index].right == prev_index) {
                    coding[tree[i].symbol].code[coding[tree[i].symbol].len] = 1;
                    coding[tree[i].symbol].len++;
                }
            }
            reverse(coding[tree[i].symbol].code, coding[tree[i].symbol].code + coding[tree[i].symbol].len);
            all_bytes += coding[tree[i].symbol].len * freq_t[tree[i].symbol];
            cnt += freq_t[tree[i].symbol];
        }
        all_bytes = (all_bytes + 7) / 8;
        fprintf(f2, "%d\n", all_bytes);
        fprintf(f2, "%d\n", cnt);
        f1 = fopen(argv[3], "rb");
        unsigned char byte = 0;
        int cnt_of_byte = 0;
        while (fscanf(f1, "%c", &ch) != -1) {
            Code temp = coding[ch];
            for (i = 0; i < temp.len; i++) {
                byte = byte << 1;
                byte += temp.code[i];
                cnt_of_byte++;
                if (cnt_of_byte == 8) {
                    fwrite(&byte, 1, 1, f2);
                    byte = 0;
                    cnt_of_byte = 0;
                }
            }
        }
        if (cnt_of_byte > 0) {
            while (cnt_of_byte < 8) {
                byte <<= 1;
                cnt_of_byte++;
            }
            fwrite(&byte, 1, 1, f2);
        }
        fclose(f1);
        fclose(f2);
    } else {
        // дальше вроде как разархиватор
        f1 = fopen(argv[2], "rb");
        f2 = fopen(argv[3], "w");
        int size_tree, all_bytes, cnt, i;
        unsigned char chc;
        Tree tr[511];

        fscanf(f1, "%d\n", &size_tree);
        for (i = 0; i < size_tree; i++) fscanf(f1, "%d %d %d %d\n", &tr[i].left, &tr[i].right, &tr[i].parent, &tr[i].symbol);
        fscanf(f1, "%d\n", &all_bytes);
        fscanf(f1, "%d\n", &cnt);
        int cur_index = size_tree - 1;
        for (i = 0; i < all_bytes; i++) {
            unsigned char maska = 128;
            unsigned char chc;
            fread(&chc, 1, 1, f1);
            while (maska) {
                if ((chc & maska) == maska) cur_index = tr[cur_index].right;
                else cur_index = tr[cur_index].left;
                maska /= 2;
                if (tr[cur_index].left == -1 && tr[cur_index].right == -1) {
                    char symb(tr[cur_index].symbol);
                    fwrite(&symb, 1, 1, f2);
                    cur_index = size_tree - 1;
                    cnt--;
                    if (cnt == 0) break;
                }
            }
        }
    }
    return 0;
}
