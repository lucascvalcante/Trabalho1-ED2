#include "svg.h"


FILE* abre_svg(const char* caminho) {
    FILE* f = fopen(caminho, "w");
    if (f) {
        fprintf(f, "<svg width=\"10000\" height=\"10000\" xmlns=\"http://www.w3.org/2000/svg\">\n");
    }
    return f;
}

void fecha_svg(FILE* f) {
    if (f) {
        fprintf(f, "</svg>\n");
        fclose(f);
    }
}

void svg_quadra(FILE* f, double x, double y, double w, double h, const char* fill, const char* strk, const char* sw) {
    fprintf(f, "  <rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" fill=\"%s\" stroke=\"%s\" stroke-width=\"%s\" />\n",
            x, y, w, h, fill, strk, sw);
}

void svg_marca_x(FILE* f, double x, double y) {
    fprintf(f, "  <line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"2\" />\n", x-5, y-5, x+5, y+5);
    fprintf(f, "  <line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"2\" />\n", x-5, y+5, x+5, y-5);
}

void svg_marca_cruz(FILE* f, double x, double y) {
    fprintf(f, "  <line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"2\" />\n", x, y-7, x, y+7);
    fprintf(f, "  <line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"2\" />\n", x-7, y, x+7, y);
}

void svg_marca_circulo(FILE* f, double x, double y) {
    fprintf(f, "  <circle cx=\"%lf\" cy=\"%lf\" r=\"5\" fill=\"none\" stroke=\"black\" stroke-width=\"2\" />\n", x, y);
}

void svg_texto(FILE* f, double x, double y, const char* txt, const char* cor, const char* anchor) {
    const char* alinhamento = (anchor != NULL) ? anchor : "start";

    fprintf(f, "  <text x=\"%lf\" y=\"%lf\" fill=\"%s\" text-anchor=\"%s\" font-size=\"12\" font-weight=\"bold\" font-family=\"sans-serif\">%s</text>\n", 
            x, y, cor, alinhamento, txt);
}

void gerar_svg_geo(const char* caminho_svg, exHash hash_quadras) {
    FILE* f_svg = fopen(caminho_svg, "w");
    if (!f_svg) {
        printf("Erro ao criar o arquivo SVG: %s\n", caminho_svg);
        return;
    }
    
    fprintf(f_svg, "<svg width=\"10000\" height=\"10000\" xmlns=\"http://www.w3.org/2000/svg\">\n");
    foreach_exHash(hash_quadras, escreve_retangulo_svg, f_svg);
    fprintf(f_svg, "</svg>\n");
    fclose(f_svg);
}

void escreve_retangulo_svg(const char* chave, void* registro, void* arquivo) {
    Quadra q = (Quadra)registro;
    FILE* f_svg = (FILE*)arquivo;

    double x = get_quadra_x(q);
    double y = get_quadra_y(q);
    double w = get_quadra_w(q);
    double h = get_quadra_h(q);
    
    fprintf(f_svg, "\t<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" "
                   "fill=\"%s\" stroke=\"%s\" stroke-width=\"%s\" />\n",
            get_quadra_x(q), get_quadra_y(q), get_quadra_w(q), get_quadra_h(q), 
            get_quadra_cfill(q), get_quadra_cstrk(q), get_quadra_sw(q));
        
    svg_texto(f_svg, x + (w / 2.0), y + (h / 2.0), chave, "black", "middle");
}