# Configurazione dell'Ambiente di Sviluppo

Questa guida spiega come configurare un ambiente virtuale con Conda, installare PyTorch e Ultralytics con YOLOv8, e avviare un server Node.js.

## Configurazione di un Venv con Conda

### Installazione di Conda
1. Scarica e installa [Anaconda](https://www.anaconda.com/products/distribution) o [Miniconda](https://docs.conda.io/en/latest/miniconda.html) seguendo le istruzioni per il tuo sistema operativo.

### Creazione di un Ambiente Virtuale
2. Crea un nuovo ambiente Conda:
    ```bash
    conda create --name myenv python=3.8
    ```

3. Attiva l'ambiente:
    ```bash
    conda activate myenv
    ```

### Installazione di PyTorch
4. Installa PyTorch:
    ```bash
    conda install pytorch torchvision torchaudio cudatoolkit=11.3 -c pytorch
    ```

### Installazione di Ultralytics con YOLOv8
5. Installa Ultralytics:
    ```bash
    pip install ultralytics
    ```

## Configurazione del Server Node.js

### Prerequisiti
1. **Node.js**: Assicurati di avere Node.js installato. Puoi scaricarlo da [Node.js](https://nodejs.org/).

### Avvio del Server
2. Crea un file `server.js`, oppure utilizzare quello contenuto nella repo.

3. Avvia il server:
    ```bash
    node server.js
    ```

## Adattamento dei Path

**Nota**: Nei vari file di configurazione, assicurati di adattare i path con i tuoi percorsi locali. Ad esempio, se stai lavorando con dataset o altri file specifici, modifica i path
