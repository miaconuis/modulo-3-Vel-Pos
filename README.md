# 🧠 MIACON - Monitor de Servo y Motor DC con ESP32 y OLED

Proyecto de instrumentación y control con **ESP32**, diseñado para prácticas de **identificación de sistemas, control y adquisición de datos**.
Este sistema permite visualizar en tiempo real los valores de **posición del servo**, **velocidad del motor DC**, y **RPM medidas con un encoder**, todo desde una **pantalla OLED SSD1306**.

---

## ⚙️ Funcionalidad General

El sistema implementa dos lazos independientes:

* **Control de posición (Servo):**
  Un potenciómetro define el **setpoint** (ángulo deseado).
  El movimiento del servo solo se actualiza al presionar un botón.

* **Control de velocidad (Motor DC):**
  Un segundo potenciómetro regula el **PWM de velocidad**.
  El valor mostrado en la pantalla corresponde al porcentaje intuitivo (0 = detenido, 255 = máxima velocidad).
  Internamente, el PWM se invierte para adaptarse a la lógica del driver de motor.

* **Medición de RPM:**
  Un sensor de **encoder** conectado a una interrupción del ESP32 mide la velocidad real del motor.

---

## 🧩 Componentes Utilizados

| Componente                                     | Descripción                                             |
| ---------------------------------------------- | ------------------------------------------------------- |
| **ESP32**                                      | Microcontrolador principal (alimentación por USB 5V)    |
| **Servo SG90 / MG90S**                         | Controlado mediante PWM (posición angular)              |
| **Motor DC + Driver (L9110 / MX1508 / L298N)** | Control de velocidad con PWM y dirección fija           |
| **Pantalla OLED SSD1306 (I2C)**                | Visualización de variables y monitoreo en tiempo real   |
| **Encoder**                                    | Sensor para medir la velocidad angular del motor        |
| **Potenciómetros (x2)**                        | Uno para posición (servo) y otro para velocidad (motor) |
| **Pulsador**                                   | Activa el movimiento del servo al presionarse           |

---

## 🪜 Conexiones Principales

| Elemento                | Pin ESP32 | Descripción                             |
| ----------------------- | --------- | --------------------------------------- |
| Servo                   | GPIO 23   | Señal PWM de posición                   |
| Potenciómetro Posición  | GPIO 34   | Entrada analógica (Setpoint servo)      |
| Potenciómetro Velocidad | GPIO 32   | Entrada analógica (Velocidad motor)     |
| Botón                   | GPIO 26   | Entrada digital con `INPUT_PULLUP`      |
| Motor PWM               | GPIO 25   | Señal PWM del motor (canal LEDC)        |
| Motor DIR               | GPIO 33   | Dirección fija (HIGH)                   |
| Encoder                 | GPIO 27   | Entrada digital con interrupción RISING |
| OLED SDA                | GPIO 21   | I2C SDA                                 |
| OLED SCL                | GPIO 22   | I2C SCL                                 |

> ⚠️ **Nota:**
> Si el motor se mueve al revés o la lógica de velocidad está invertida, puedes intercambiar los pines **A-1A ↔ A-1B** del driver en lugar de modificar el código.

---

## 🧮 Parámetros Importantes

* **Frecuencia PWM:** 1 kHz
* **Resolución PWM:** 8 bits (0–255)
* **Lecturas analógicas:** 0–4095
* **Encoder:** 20 pulsos por revolución
* **Actualización OLED:** cada 200 ms

---

## 📊 Variables Visualizadas en OLED

| Línea | Variable                                                         | Descripción |
| ----- | ---------------------------------------------------------------- | ----------- |
| 1     | **MIACON - MONITOR**                                             | Título      |
| 2     | **Setpoint:** Valor en tiempo real del potenciómetro de posición |             |
| 3     | **Servo:** Último valor aplicado tras pulsar el botón            |             |
| 4     | **PWM:** Nivel de velocidad (0–255)                              |             |
| 5     | **RPM:** Velocidad medida mediante encoder                       |             |

---

## 🧠 Flujo de Operación

1. Encender el sistema (alimentación por USB 5V).
2. Ajustar el **potenciómetro de posición** → se actualiza el setpoint en pantalla.
3. Presionar el **botón** → el servo se mueve al nuevo ángulo.
4. Ajustar el **potenciómetro de velocidad** → el motor cambia de velocidad.
5. Observar las **RPM medidas** y el **PWM aplicado** en la pantalla OLED.

---

## 🔬 Aplicación Académica

Este módulo está diseñado para ser utilizado dentro del proyecto educativo **MIACON**, permitiendo:

* Identificación experimental de sistemas (planta motor/servo).
* Diseño de controladores PID, fuzzy o adaptativos.
* Integración con herramientas como **Python, Scilab o Simulink** para modelado y análisis.
* Registro de señales mediante comunicación serial para análisis en software externo.

---

## 🧾 Dependencias de Librerías

Asegúrate de tener instaladas las siguientes librerías en el **Arduino IDE**:

```bash
Adafruit GFX Library
Adafruit SSD1306
ESP32Servo
```

Todas disponibles desde el **Administrador de Librerías** del IDE.

---

## 🚀 Compilación y Carga

1. Abrir el proyecto en **Arduino IDE** o **PlatformIO**.
2. Seleccionar la placa:
   **ESP32 Dev Module**
3. Conectar por **USB** y seleccionar el puerto COM correspondiente.
4. Compilar y subir el código.
5. Observar la inicialización en la pantalla OLED.

---

Proyecto **MIACON**
📘 *Módulo de Instrumentación y Control Experimental con ESP32*

Repositorio: [https://github.com/miaconuis/modulo-3-Vel-Pos)

---

## 📸 Ejemplo de Pantalla

```
MIACON - MONITOR
----------------
Setpoint: 120°
Servo:     120°
PWM:        180
RPM:        56.2
```

---
