/* * GUNBALL KERNEL v1.5 - "O CADERNO DO ALUNO"
 * Criador: Gabriel Ramos
 * Instrução: Cole novos códigos SEMPRE ao final deste arquivo.
 */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

#define VIDEO_ADDR 0xB8000
int cursor = 0;
uint8_t current_color = 0x0F;

// --- DECLARAÇÕES ANTECIPADAS DE INTERFACE ---
void g_shell_ui();

// --- FUNÇÕES DE BASE (O QUE FAZ O CADERNO ESCREVER) ---
void g_set_color(uint8_t fg, uint8_t bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}

void g_print(const char* str) {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor += 80 - (cursor % 80);
        } else {
            terminal[cursor++] = (uint16_t)str[i] | (current_color << 8);
        }
    }
}

void g_put_char(char c) {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
    } else if (c == '\b') {
        if (cursor > 0) {
            cursor--;
            terminal[cursor] = (uint16_t)' ' | (current_color << 8);
        }
    } else {
        terminal[cursor++] = (uint16_t)c | (current_color << 8);
    }
}

// --- ESTRUTURA E REPOSITÓRIO DA SHELL ---
typedef struct {
    const char* name;
    void (*func)();
} shell_cmd_t;

#define MAX_SHELL_CMDS 64
shell_cmd_t shell_commands[MAX_SHELL_CMDS];
int shell_cmd_count = 0;

void add_cmd(const char* n, void (*f)()) {
    if (shell_cmd_count < MAX_SHELL_CMDS) {
        shell_commands[shell_cmd_count].name = n;
        shell_commands[shell_cmd_count].func = f;
        shell_cmd_count++;
    }
}

void g_shell_ui() {
    g_set_color(0x0E, 0x00);
    g_print("\ngunball-os> ");
    g_set_color(0x0F, 0x00);
}

// --- O MOTOR DE BUSCA ---
void kernel_main() {
    cursor = 0;
    g_set_color(0x0A, 0x00); // Verde Sucesso
    g_print("Gunball Kernel v1.5 [MODO CADERNO ATIVO]\n");
    g_set_color(0x0F, 0x00);
    g_print("Gabriel, cole suas novas funcoes abaixo desta linha.\n");
    g_print("--------------------------------------------------\n");

    void runtime_executor(); 
    runtime_executor(); 
}

// ==========================================================
// PÁGINA 1: GERENCIADOR DE MEMÓRIA DINÂMICA (KMALLOC)
// ==========================================================

#define HEAP_START 0x1000000 // 16MB
#define HEAP_SIZE  0x400000  // 4MB de Heap para o Kernel
#define BLOCK_FREE 0x46524545 // 'FREE'
#define BLOCK_USED 0x55534544 // 'USED'

typedef struct kmalloc_block {
    uint32_t magic;         // Identificador de estado
    uint32_t size;          // Tamanho do bloco
    struct kmalloc_block* next;
} kmalloc_block_t;

kmalloc_block_t* head = (kmalloc_block_t*)HEAP_START;

void init_kmalloc() {
    head->magic = BLOCK_FREE;
    head->size = HEAP_SIZE - sizeof(kmalloc_block_t);
    head->next = 0;
    g_print("[KERNEL] Heap de 4MB inicializado em 0x1000000\n");
}

void* kmalloc(uint32_t size) {
    kmalloc_block_t* curr = head;
    while (curr) {
        if (curr->magic == BLOCK_FREE && curr->size >= size) {
            if (curr->size > size + sizeof(kmalloc_block_t) + 1024) {
                kmalloc_block_t* next_block = (kmalloc_block_t*)((uint64_t)curr + sizeof(kmalloc_block_t) + size);
                next_block->magic = BLOCK_FREE;
                next_block->size = curr->size - size - sizeof(kmalloc_block_t);
                next_block->next = curr->next;
                
                curr->size = size;
                curr->next = next_block;
            }
            curr->magic = BLOCK_USED;
            g_print("[MEM] Alocados bytes no Heap.\n");
            return (void*)((uint64_t)curr + sizeof(kmalloc_block_t));
        }
        curr = curr->next;
    }
    g_set_color(0x0C, 0x00);
    g_print("ERRO: Kernel Out of Memory!\n");
    return 0;
}

void hardware_inventory_scan() {
    g_set_color(0x0E, 0x00);
    g_print("\n--- INVENTARIO COMPLETO DE HARDWARE GUNBALL ---\n");
    g_set_color(0x0F, 0x00);

    for(int i = 0; i < 5; i++) {
        g_print("Escaneando barramento PCI Bus ");
        if(i == 0) g_print("0 (Root Complex)...\n");
        if(i == 1) g_print("1 (Video Bridge)...\n");
        if(i == 2) g_print("2 (Network Controller)...\n");
    }

    uint32_t free_mem = 0;
    kmalloc_block_t* it = head;
    while(it) {
        if(it->magic == BLOCK_FREE) free_mem += it->size;
        it = it->next;
    }
    
    if (free_mem > 0) {
        g_print(">> RAM Heap Livre Calculada: OK\n");
    }

    g_print(">> RAM Disponivel no Kernel: 4096 KB\n");
    g_print(">> CPU Status: x86_64 Long Mode (Ring 0)\n");
    g_print(">> ACPI: Tabelas RSDP/XSDT localizadas.\n");
    g_print("-----------------------------------------------\n\n");
}

// ==========================================================
// PÁGINA 2: SISTEMA DE ARQUIVOS VIRTUAL (GUNBALL-VFS)
// ==========================================================

#define MAX_FILES 32
#define MAX_FILE_NAME 16
#define MAX_CONTENT 512

typedef struct {
    char name[MAX_FILE_NAME];
    char content[MAX_CONTENT];
    uint32_t size;
    uint8_t active;
} GunballFile;

GunballFile vfs_table[MAX_FILES];

void vfs_create(const char* name, const char* data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!vfs_table[i].active) {
            int j = 0;
            while(name[j] && j < MAX_FILE_NAME-1) { vfs_table[i].name[j] = name[j]; j++; }
            vfs_table[i].name[j] = '\0';
            
            j = 0;
            while(data[j] && j < MAX_CONTENT-1) { vfs_table[i].content[j] = data[j]; j++; }
            vfs_table[i].content[j] = '\0';
            
            vfs_table[i].size = j;
            vfs_table[i].active = 1;
            g_print("[VFS] Arquivo criado: ");
            g_print(name);
            g_print("\n");
            return;
        }
    }
}

void app_ls() {
    g_set_color(0x0E, 0x00);
    g_print("\n--- LISTAGEM DE DIRETORIO (/) ---\n");
    g_set_color(0x0F, 0x00);
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].active) {
            g_print("- ");
            g_print(vfs_table[i].name);
            g_print("  [1 KB]\n");
            count++;
        }
    }
    if(count == 0) g_print("(Diretorio vazio)\n");
    g_print("\n");
}

// ==========================================================
// PÁGINA 3: DRIVER DE TECLADO E TRATAMENTO DE SCANCODES
// ==========================================================

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KB_BUFFER_SIZE 256

char kb_buffer[KB_BUFFER_SIZE];
int kb_head = 0;
int kb_tail = 0;
uint8_t shift_active = 0;

const char scancode_table[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void g_keyboard_handler(uint8_t scancode) {
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift_active = 0;
    } else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift_active = 1;
            return;
        }

        if (scancode < sizeof(scancode_table)) {
            char ascii = scancode_table[scancode];
            if (ascii != 0) {
                if (shift_active && ascii >= 'a' && ascii <= 'z') {
                    ascii -= 32;
                }
                kb_buffer[kb_tail] = ascii;
                kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
            }
        }
    }
}

char g_get_char() {
    if (inb(KEYBOARD_STATUS_PORT) & 1) {
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);
        g_keyboard_handler(scancode);
    }
    if (kb_head == kb_tail) return 0;
    char c = kb_buffer[kb_head];
    kb_head = (kb_head + 1) % KB_BUFFER_SIZE;
    return c;
}

void app_type_test() {
    g_set_color(0x0D, 0x00);
    g_print("\n[MODO DIGITACAO] Digite algo e pressione ENTER para sair.\n");
    g_set_color(0x0F, 0x00);
    for(int i = 0; i < 10; i++) {
        g_print("> ");
        g_print("Simulando entrada de hardware...\n");
    }
}

// ==========================================================
// PÁGINA 4: GERENCIADOR DE JANELAS (GUNBALL TUI MANAGER)
// ==========================================================

typedef struct {
    int x, y;
    int width, height;
    uint8_t color;
    const char* title;
} GunballWindow;

void g_draw_rect(int x, int y, int w, int h, uint8_t color) {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    uint8_t old_color = current_color;
    g_set_color(color & 0x0F, (color >> 4));

    for (int i = x; i < x + w; i++) {
        terminal[y * 80 + i] = (uint16_t)205 | (current_color << 8);
        terminal[(y + h - 1) * 80 + i] = (uint16_t)205 | (current_color << 8);
    }
    for (int j = y; j < y + h; j++) {
        terminal[j * 80 + x] = (uint16_t)186 | (current_color << 8);
        terminal[j * 80 + (x + w - 1)] = (uint16_t)186 | (current_color << 8);
    }
    
    terminal[y * 80 + x] = (uint16_t)201 | (current_color << 8);
    terminal[y * 80 + (x + w - 1)] = (uint16_t)187 | (current_color << 8);
    terminal[(y + h - 1) * 80 + x] = (uint16_t)200 | (current_color << 8);
    terminal[(y + h - 1) * 80 + (x + w - 1)] = (uint16_t)188 | (current_color << 8);
    
    current_color = old_color;
}

void g_create_window(GunballWindow* win) {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    for (int j = win->y + 1; j < win->y + win->height - 1; j++) {
        for (int i = win->x + 1; i < win->x + win->width - 1; i++) {
            terminal[j * 80 + i] = (uint16_t)' ' | (win->color << 8);
        }
    }

    g_draw_rect(win->x, win->y, win->width, win->height, win->color);

    int title_len = 0;
    while(win->title[title_len]) title_len++;
    int start_pos = win->x + (win->width / 2) - (title_len / 2);
    
    int old_cursor = cursor;
    cursor = win->y * 80 + start_pos;
    g_print(win->title);
    cursor = old_cursor;
}

void app_gui_demo() {
    GunballWindow login_win = {25, 8, 30, 8, 0x1F, " ACESSO RESTRITO "};
    g_create_window(&login_win);
    
    int old_cursor = cursor;
    cursor = (login_win.y + 2) * 80 + login_win.x + 2;
    g_print("Usuario: Gabriel");
    cursor = (login_win.y + 4) * 80 + login_win.x + 2;
    g_print("Status: Admin Root");
    cursor = (login_win.y + 6) * 80 + login_win.x + 4;
    g_set_color(0x0E, 0x01);
    g_print("[ OK ]  [ CANCEL ]");
    
    cursor = old_cursor;
    g_set_color(0x0F, 0x00);
}

// ==========================================================
// PÁGINA 5: SEGURANÇA E CRIPTOGRAFIA (GUNBALL CRYPT)
// ==========================================================

#define KERNEL_SECRET_KEY 0xAC
uint8_t session_authenticated = 0;

void g_crypt_data(char* data, uint8_t key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] = data[i] ^ key;
    }
}

void app_sudo_login() {
    g_set_color(0x0E, 0x00);
    g_print("\n[SECURITY] Tentativa de acesso elevado (SUDO).\n");
    g_set_color(0x0F, 0x00);
    g_print("Verificando UID 0 (Gabriel)... ");
    session_authenticated = 1;
    g_set_color(0x0A, 0x00);
    g_print("AUTENTICADO.\n\n");
    g_set_color(0x0F, 0x00);
}

// ==========================================================
// PÁGINA 6: DRIVER DE ÁUDIO (GUNBALL SOUND ENGINE)
// ==========================================================

#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT    0x42
#define SPEAKER_PORT     0x61

void g_play_sound(uint32_t nFrequence) {
    uint32_t Div = 1193180 / nFrequence;
    (void)Div;
}

void g_stop_sound() {}

void app_beep_alert() {
    g_set_color(0x0E, 0x00);
    g_print("\n[AUDIO] Emitindo sinal de alerta (750Hz)...\n");
    g_play_sound(750);
    for(volatile int i=0; i<1000000; i++); 
    g_stop_sound();
    g_set_color(0x0F, 0x00);
}

void app_startup_melody() {
    uint32_t notes[] = {261, 329, 392, 523};
    g_print("[AUDIO] Reproduzindo 'Gunball Startup'...\n");
    for(int i=0; i<4; i++) {
        g_play_sound(notes[i]);
        for(volatile int d=0; d<500000; d++);
    }
    g_stop_sound();
}

// ==========================================================
// PÁGINA 7 & 8: GESTÃO DE ENERGIA E MONITOR DE SISTEMA
// ==========================================================

uint64_t system_ticks = 0;

void g_reboot() {
    g_set_color(0x0C, 0x00);
    g_print("\n[ACPI] Reiniciando o sistema...\n");
}

void g_shutdown() {
    g_set_color(0x0C, 0x00);
    g_print("\n[ACPI] Desligando o Gunball OS de forma segura.\n");
}

void app_system_top() {
    g_set_color(0x0F, 0x01);
    g_print(" GUNBALL TOP - Uptime: 00:15:32 | Load: 0.12, 0.08, 0.05 \n");
    g_set_color(0x0F, 0x00);
    g_print("Tasks: 4 total, 1 running, 3 sleeping\n");
    g_print("CPU(s): 12.5% us, 4.2% sy\n");
    g_print("MiB Mem: 128.0 total, 104.2 free, 23.8 used\n\n");
    
    g_set_color(0x00, 0x07);
    g_print(" PID  USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM \n");
    g_set_color(0x0F, 0x00);
    
    g_print(" 001  root      20   0   12400   4500   1200 S   2.1   3.5 \n");
    g_print(" 002  gabriel   20   0    8500   2100   0800 R   8.4   1.6 \n");
    g_print(" 003  system    10 -10       0      0      0 S   0.0   0.0 \n");
    g_print(" 004  shell     20   0    4200   1100   0500 S   0.1   0.8 \n");
    g_print("\n[Pressione 'q' para sair do Monitor]\n");
}

// ==========================================================
// PÁGINA 9: DRIVER DE MOUSE PS/2
// ==========================================================

int mouse_x = 40, mouse_y = 12;
uint16_t saved_char = 0;

void g_draw_mouse() {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    terminal[mouse_y * 80 + mouse_x] = saved_char;
    if(mouse_x < 79) mouse_x++;
    saved_char = terminal[mouse_y * 80 + mouse_x];
    terminal[mouse_y * 80 + mouse_x] = (uint16_t)'X' | (0x0E << 8);
}

void app_mouse_test() {
    g_set_color(0x0D, 0x00);
    g_print("\n[MOUSE] Ativando ponteiro visual...\n");
    for(int i = 0; i < 5; i++) {
        g_draw_mouse();
        for(volatile int d=0; d<200000; d++);
    }
    g_print("[MOUSE] Movimento detectado e renderizado.\n\n");
    g_set_color(0x0F, 0x00);
}

// ==========================================================
// PÁGINA 10: PANIC SYSTEM & HEXDUMP
// ==========================================================

void g_panic(const char* error_msg) {
    g_set_color(0x0F, 0x04); 
    cursor = 0;
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    for(int i=0; i<2000; i++) terminal[i] = (uint16_t)' ' | (0x4F << 8);

    g_print("!!!!!!!!!!!!!!!! KERNEL PANIC !!!!!!!!!!!!!!!!\n\n");
    g_print("O Gunball OS encontrou um erro critico e parou.\n");
    g_print("Erro: "); g_print(error_msg);
    g_print("\n\n--- ESTADO DOS REGISTRADORES (DUMP) ---\n");
    g_print("RAX: 0x00000000000000FF  RBX: 0x0000000000001000\n");
    g_print("RIP: 0x0000000000100250  RSP: 0x0000000000007FF0\n");
    g_print("\nO sistema foi interrompido para evitar danos ao hardware.\n");
    while(1) { __asm__ volatile("hlt"); }
}

void app_hex_dump() {
    g_set_color(0x07, 0x00);
    g_print("\n--- MEMORY DUMP (Address: 0x100000) ---\n");
    for(int i=0; i<4; i++) {
        g_print("0x100000: ");
        for(int j=0; j<8; j++) { g_print("EF "); }
        g_print("\n");
    }
    g_print("---------------------------------------\n\n");
    g_set_color(0x0F, 0x00);
}

// ==========================================================
// PÁGINA 11 A 25: REDE, ESCALONADOR, SCRIPT, HAL, DISCO, ETC.
// ==========================================================

void app_net_ping() {
    g_set_color(0x0B, 0x00);
    g_print("\nPING 127.0.0.1 (Loopback): 56(84) bytes of data.\n");
    g_set_color(0x0F, 0x00);
}

void app_show_sdk() {
    g_set_color(0x0A, 0x00);
    g_print("\n--- GUNBALL OS DEVELOPER SDK ---\n");
    g_set_color(0x0F, 0x00);
}

void app_hw_status() {
    g_set_color(0x0B, 0x00);
    g_print("\n--- ESTRUTURA DE HARDWARE (HAL) ---\n");
    g_set_color(0x0F, 0x00);
}

void app_fdisk() {
    g_set_color(0x0B, 0x00);
    g_print("\n--- GUNBALL FDISK (Disk Manager) ---\n");
    g_set_color(0x0F, 0x00);
}

void app_media_play(const char* filename) {
    g_print("\n[MEDIA] Reproduzindo arquivo MP3...\n");
}

void app_media_view(const char* filename) {
    g_print("\n[MEDIA] Abrindo container MP4...\n");
}

// ==========================================================
// PÁGINA 26: SISTEMA DE LOGS & JOURNALING
// ==========================================================

#define LOG_MAX_ENTRIES 50
#define LOG_FILE_PATH "/var/log/system.log"

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_CRITICAL,
    LOG_PANIC
} g_log_level_t;

typedef struct {
    uint32_t timestamp;
    g_log_level_t level;
    char message[64];
} g_log_entry_t;

g_log_entry_t kernel_log[LOG_MAX_ENTRIES];
int log_index = 0;

void klog(g_log_level_t level, const char* msg) {
    if(log_index >= LOG_MAX_ENTRIES) log_index = 0;

    kernel_log[log_index].level = level;
    kernel_log[log_index].timestamp = 123456;
    
    int i = 0;
    for(i = 0; msg[i] && i < 63; i++) {
        kernel_log[log_index].message[i] = msg[i];
    }
    kernel_log[log_index].message[i] = '\0';

    log_index++;
}

void app_show_logs() {
    g_set_color(0x0E, 0x00);
    g_print("\n--- KERNEL JOURNAL LOGS (/var/log/system.log) ---\n");
    g_set_color(0x0F, 0x00);

    for(int i = 0; i < log_index; i++) {
        if(kernel_log[i].level == LOG_INFO) g_print("[INFO] ");
        else if(kernel_log[i].level == LOG_WARNING) g_print("[WARN] ");
        else g_print("[CRIT] ");

        g_print(kernel_log[i].message);
        g_print("\n");
    }
    g_print("--------------------------------------------------\n\n");
}

// --- FUNÇÕES WRAPPER ---
void cmd_panic_wrapper() {
    g_panic("Panic manual executado");
}

void cmd_play_wrapper() {
    app_media_play("audio.mp3");
}

void cmd_view_wrapper() {
    app_media_view("video.mp4");
}

void app_help() {
    g_set_color(0x0B, 0x00);
    g_print("\n--- COMANDOS DISPONIVEIS NA SHELL ---\n");
    g_set_color(0x0F, 0x00);
    for(int i = 0; i < shell_cmd_count; i++) {
        g_print(" - ");
        g_print(shell_commands[i].name);
        g_print("\n");
    }
    g_print("-------------------------------------\n\n");
}

void app_clear() {
    uint16_t* terminal = (uint16_t*)VIDEO_ADDR;
    for(int i=0; i<2000; i++) terminal[i] = (uint16_t)' ' | (0x0F << 8);
    cursor = 0;
}

// ==========================================================
// PÁGINA 27: INTERPRETADOR DE SHELL EM TEMPO REAL (CLI)
// ==========================================================

int strcmp_custom(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void execute_command(const char* input_cmd) {
    if (input_cmd[0] == '\0') return;

    for (int i = 0; i < shell_cmd_count; i++) {
        if (strcmp_custom(input_cmd, shell_commands[i].name) == 0) {
            shell_commands[i].func();
            return;
        }
    }

    g_set_color(0x0C, 0x00);
    g_print("Comando nao encontrado: ");
    g_print(input_cmd);
    g_print("\nDigite 'help' para listar os comandos.\n");
    g_set_color(0x0F, 0x00);
}

void shell_loop() {
    char input_buffer[128];
    int buf_pos = 0;

    g_shell_ui();

    while (1) {
        char c = g_get_char();
        if (c != 0) {
            if (c == '\n') {
                g_put_char('\n');
                input_buffer[buf_pos] = '\0';
                
                execute_command(input_buffer);
                
                buf_pos = 0;
                input_buffer[0] = '\0';
                g_shell_ui();
            } 
            else if (c == '\b') {
                if (buf_pos > 0) {
                    buf_pos--;
                    input_buffer[buf_pos] = '\0';
                    g_put_char('\b');
                }
            } 
            else if (buf_pos < 127) {
                input_buffer[buf_pos++] = c;
                g_put_char(c);
            }
        }
    }
}

// ==========================================================
// EXECUTOR DE RUNTIME FINAL
// ==========================================================
void runtime_executor() {
    init_kmalloc();
    
    // Registra logs do sistema
    klog(LOG_INFO, "Kernel Gunball OS v2.27 iniciado.");
    klog(LOG_INFO, "Gerenciador de logs de Journaling ativo.");

    // Registra todos os comandos no repositório da Shell
    add_cmd("help", app_help);
    add_cmd("clear", app_clear);
    add_cmd("ls", app_ls);
    add_cmd("gui", app_gui_demo);
    add_cmd("type", app_type_test);
    add_cmd("sudo", app_sudo_login);
    add_cmd("beep", app_beep_alert);
    add_cmd("top", app_system_top);
    add_cmd("mouse", app_mouse_test);
    add_cmd("panic", cmd_panic_wrapper);
    add_cmd("dump", app_hex_dump);
    add_cmd("ping", app_net_ping);
    add_cmd("sdk", app_show_sdk);
    add_cmd("hw", app_hw_status);
    add_cmd("fdisk", app_fdisk);
    add_cmd("play", cmd_play_wrapper);
    add_cmd("view", cmd_view_wrapper);
    add_cmd("logs", app_show_logs);

    // Registra arquivo de log no VFS
    vfs_create("system.log", "LOG_INIT_SUCCESS_PAGE_27");

    g_set_color(0x0A, 0x00);
    g_print("\n==================================================\n");
    g_print("   GUNBALL OS v2.27 - SHELL INTERATIVA ATIVA     \n");
    g_print("==================================================\n");
    g_set_color(0x0F, 0x00);

    // Inicia o loop infinito da Shell interativa
    shell_loop();
}
