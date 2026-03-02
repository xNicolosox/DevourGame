
# 🔦 UNIFAP: Terror em computação ( OpenGL Engine)

Este projeto é um jogo de **Survival Horror em Primeira Pessoa**, construído do zero utilizando uma engine customizada em **C++ e OpenGL (pipeline fixo + GLSL 1.20)**. O gerenciamento de janela e entrada é feito via **GLUT**, funções modernas (shaders, VBOs) via **GLEW**, e o sistema de áudio 3D imersivo utiliza **OpenAL**.

---

# 🛠️ Setup Completo do Ambiente (Windows + MSYS2)

Este guia explica como configurar um ambiente completo para desenvolvimento em C/C++ com OpenGL, utilizando **MSYS2, FreeGLUT, GLEW, OpenAL e FreeALUT** no Windows.

---

## 📦 1. Instalação do MSYS2

### 🔹 Baixar e instalar
1. Acesse: https://www.msys2.org  
2. Baixe a versão para **Windows 64 bits**.  
3. Conclua a instalação padrão.

### 🔹 Atualizar o sistema
1. Abra o terminal **MSYS2 MSYS** (ícone roxo).
2. Execute:

```bash
pacman -Syu
```

Aceite todas as atualizações.

---

## 🔧 2. Instalar GCC, GDB e Bibliotecas Gráficas

Feche o terminal anterior e abra o terminal **MSYS2 MinGW 64-bit** (ícone azul).

Execute os comandos abaixo:

### 🧠 Compilador C/C++

```bash
pacman -S mingw-w64-x86_64-gcc
```

### 🐞 Debugger

```bash
pacman -S mingw-w64-x86_64-gdb
```

### 🖥️ FreeGLUT (Janelas e Input)

```bash
pacman -S mingw-w64-x86_64-freeglut
```

### ⚡ GLEW (Extensões modernas do OpenGL)

```bash
pacman -S mingw-w64-x86_64-glew
```

### 🔊 OpenAL (Motor de Áudio 3D)

```bash
pacman -S mingw-w64-x86_64-openal
```

### 🎵 FreeALUT (Carregamento de arquivos de som)

```bash
pacman -S mingw-w64-x86_64-freealut
```

---

## 🛠️ 3. Configurar Variáveis de Ambiente

Para que o Windows reconheça os comandos `gcc` e `g++`:

1. Abra a busca do Windows e digite:  
   **"Editar as variáveis de ambiente do sistema"**
2. Clique em **Variáveis de Ambiente**.
3. Na seção **Variáveis de usuário**, selecione `Path` → clique em **Editar**.
4. Clique em **Novo** e adicione:

```
C:\msys64\mingw64\bin
```

5. Clique em **OK** em todas as janelas.

⚠️ **Importante:** Reinicie o computador (ou faça logoff).

---

## ✅ Testar Instalação

Abra o **CMD** ou **PowerShell** e digite:

```bash
gcc --version
gdb --version
```

Se aparecer a versão instalada, está tudo correto.

---

# 🎯 Objetivo do Jogo

O jogador encontra-se preso em um labirinto de servidores corrompidos e está sendo caçado.

## Mecânica Principal

1. Localizar **10 HDs** espalhados proceduralmente pelo mapa.  
2. Transportar **um HD de cada vez** até o **Incinerador**.  
3. Destruí-los.

⚠️ **Dificuldade Dinâmica:**  
A cada HD destruído, a velocidade de perseguição do Boss aumenta em **20%**.  
Contato físico com o inimigo = **Hit Kill (Morte Instantânea)**.

---

# 📦 Dependências

- `g++`
- `make`
- `freeglut`
- `glew`
- `OpenAL`
- `freealut`
- `mesa` / `glu`
- `stb_image.h` (inclusa no projeto)

---

# 🚀 Como Compilar e Executar

## 🐧 Linux

```bash
g++ src/core/*.cpp src/graphics/*.cpp src/input/*.cpp src/level/*.cpp src/audio/*.cpp src/utils/*.cpp \
    -o DevourMarcoLeal \
    -lGLEW -lGL -lGLU -lglut -lopenal -lalut && ./DevourMarcoLeal
```

## 🪟 Windows (MSYS2 / MinGW)

```bash
g++ src/core/*.cpp src/graphics/*.cpp src/input/*.cpp src/level/*.cpp src/audio/*.cpp src/utils/*.cpp ^
    -o DevourMarcoLeal.exe ^
    -lglew32 -lfreeglut -lopengl32 -lglu32 -lOpenAL32 -lalut && DevourMarcoLeal.exe
```

### 📦 Execução Standalone

Para distribuir:

- `DevourMarcoLeal.exe`
- `freeglut.dll`
- `OpenAL32.dll`
- `glew32.dll`
- Pastas:
  - `maps/`
  - `assets/`
  - `shaders/`

---

# ⌨️ Controles

| Tecla | Função |
|-------|--------|
| W, A, S, D | Movimentação |
| Mouse | Rotação da câmera |
| F | Alternar Lanterna |
| E | Interagir / Queimar HD |
| Tab | Pausar |
| Alt + Enter | Tela cheia |
| ESC | Sair |

---

# 🗺️ Sistema de Mapas (.txt)

Cada mapa é uma matriz ASCII dentro da pasta `maps/`.

Cada caractere representa um **tile 4x4 unidades**.

## ✅ Regras do Mapa

- Extensão `.txt`
- Linhas com mesmo comprimento
- Obrigatório:
  - 10x `H`
  - 1x `9`
  - 1x `E`

---

# 🧱 Legenda de Geometria

| Caractere | Estrutura |
|------------|------------|
| 1 | Parede Externa |
| 2 | Parede Interna |
| U, I, O, Y | Servidores |
| 0 | Chão Outdoor |
| 3 | Indoor (com teto) |
| L | Lava |
| B | Sangue |

---

# 👾 Entidades

| Caractere | Entidade |
|------------|------------|
| E | Spawn Player |
| 9 | Incinerador |
| H | Hard Drive |
| M | Boss Marco |
| J | Boss Júlio |
| T | Monstro Genérico |

Entidades são convertidas automaticamente para chão no grid de colisão após spawn.

---

# ⚙️ Sistema de Culling (Otimização)

A engine utiliza **Raycast + Frustum Culling (plano XZ)**.

```cpp
static float gCullHFovDeg      = 170.0f;
static float gCullNearTiles    = 2.0f;
static float gCullMaxDistTiles = 20.0f;
```

### Parâmetros

- **gCullHFovDeg** → Cone de visão principal  
- **gCullNearTiles** → Override para tiles próximos  
- **gCullMaxDistTiles** → Limite máximo de renderização (fog + otimização)