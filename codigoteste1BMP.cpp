#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}
color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

// Função para escrever um número de 16 bits em ordem little-endian no arquivo BMP.
void write_u16(std::ofstream& out, uint16_t value) {
    out.put(static_cast<char>(value & 0xFF));
    out.put(static_cast<char>((value >> 8) & 0xFF));
}

// Função para escrever um número de 32 bits em ordem little-endian no arquivo BMP.
void write_u32(std::ofstream& out, uint32_t value) {
    out.put(static_cast<char>(value & 0xFF));
    out.put(static_cast<char>((value >> 8) & 0xFF));
    out.put(static_cast<char>((value >> 16) & 0xFF));
    out.put(static_cast<char>((value >> 24) & 0xFF));
}

int main() {
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;
    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    std::ofstream bmp_file("saida.bmp", std::ios::binary);

    // Cabeçalho BMP
    uint32_t bmp_size = static_cast<uint32_t>(54 + (image_width * 3 + 4 - (image_width * 3) % 4) * image_height);
    bmp_file.put('B').put('M'); // Tipo de arquivo BMP
    write_u32(bmp_file, bmp_size); // Tamanho do arquivo
    write_u32(bmp_file, 0); // Reservado
    write_u32(bmp_file, 54); // Offset para os dados da imagem
    write_u32(bmp_file, 40); // Tamanho do cabeçalho de informações
    write_u32(bmp_file, image_width); // Largura
    write_u32(bmp_file, image_height); // Altura
    write_u16(bmp_file, 1); // Planos de cores
    write_u16(bmp_file, 24); // Profundidade de bits (24 para RGB de 8 bits)
    write_u32(bmp_file, 0); // Compressão
    write_u32(bmp_file, 0); // Tamanho da imagem (0 por padrão)
    write_u32(bmp_file, 0); // Resolução horizontal (pixels por metro)
    write_u32(bmp_file, 0); // Resolução vertical (pixels por metro)
    write_u32(bmp_file, 0); // Cores usadas (0 para máximo)
    write_u32(bmp_file, 0); // Cores importantes (0 para todas)

    // Escreva os dados de pixel na ordem BGR
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel_color = ray_color(r);
            bmp_file.put(static_cast<char>(255.999 * pixel_color.z())); // Red
            bmp_file.put(static_cast<char>(255.999 * pixel_color.y())); // Green
            bmp_file.put(static_cast<char>(255.999 * pixel_color.x())); // Blue
            
            
        }
        // Preencha com zeros para garantir que cada linha tenha um múltiplo de 4 bytes
        for (int p = 0; p < (4 - (image_width * 3) % 4) % 4; ++p) {
            bmp_file.put(0);
        }
    }

    bmp_file.close();

    std::cerr << "\nImagem BMP salva como 'saida.bmp'.\n";

    return 0;
}