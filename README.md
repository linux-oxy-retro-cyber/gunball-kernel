# gunball-kernel
um kernel com um bash embutido pode fazer tudo pra compilar use meke na pasta e para rodar make run
# 🚀 Gunball OS (Gunball Kernel)

O **Gunball OS** é um microkernel modular educacional desenvolvido em C e Assembly x86. O projeto simula o funcionamento de um sistema operacional completo, contando com gerenciamento de memória, VFS (Virtual File System), drivers de áudio/vídeo, TUI (Interface Gráfica de Texto) e uma Shell interativa em tempo real.

---

## 📷 Screenshots do Sistema

### Shell Interativa e Terminal![Gunball OS Panic](https://raw.githubusercontent.com/linux-oxy-retro-cyber/gunball-kernel/main/imagens/Captura%20de%20tela%20de%202026-07-31%2001-17-05.png)

### Sistema de Kernel Panic (BSOD/Panic Screen)
![Gunball OS Panic](https://raw.githubusercontent.com/linux-oxy-retro-cyber/gunball-kernel/main/imagens/Captura%20de%20tela%20de%202026-07-31%2001-17-14.png)
---

## 💻 Comandos da Shell CLI

A Shell interativa do Gunball OS permite executar diversos subsistemas do kernel diretamente do terminal. Veja a lista dos comandos disponíveis:

| Comando | Descrição |
| :--- | :--- |
| `help` | Exibe a lista com todos os comandos disponíveis no sistema. |
| `clear` | Limpa a tela do terminal VGA. |
| `ls` | Lista os arquivos e diretórios virtuais criados no **Gunball-VFS**. |
| `gui` | Abre a demonstração da interface gráfica TUI (Gerenciador de Janelas em modo texto). |
| `type` | Ativa o modo de teste para captura e digitação do teclado. |
| `sudo` | Executa a rotina de autenticação e elevação de privilégios para usuário Root. |
| `beep` | Emite um sinal sonoro de alerta utilizando o alto-falante da placa-mãe (PC Speaker / PIT 8254). |
| `top` | Abre o monitor de processos em tempo real (exibe CPU, RAM e PIDs ativos). |
| `mouse` | Ativa e testa a renderização do ponteiro do mouse PS/2 na tela. |
| `panic` | Força um **Kernel Panic** proposital para exibir o dump de registradores e tela de erro crítico. |
| `dump` | Exibe o hexdump bruto de um endereço de memória física (Memory Dump). |
| `ping` | Simula um teste de conectividade de rede no endereço de loopback (`127.0.0.1`). |
| `sdk` | Exibe as informações e ferramentas do Developer SDK do Gunball OS. |
| `hw` | Realiza a varredura e lista a estrutura do hardware (HAL - Hardware Abstraction Layer). |
| `fdisk` | Abre o gerenciador de partições e discos virtuais. |
| `play` | Simula a reprodução de arquivos de áudio (.mp3). |
| `view` | Simula a abertura e renderização de contêineres de vídeo (.mp4). |
| `logs` | Exibe o histórico de logs e eventos do sistema mantidos em `/var/log/system.log`. |

---

## 🛠️ Como Compilar e Rodar

### Pré-requisitos
* GCC (suporte a 32-bit com `-m32`)
* NASM
* QEMU (`qemu-system-i386`)
* Make

### Compilação
```bash
make
