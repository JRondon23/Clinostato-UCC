# Clinostato / Máquina de Posicionamiento Aleatorio (RPM) 3D

## Acerca del Proyecto
Este repositorio contiene el código fuente, diseños mecánicos, esquemáticos electrónicos y documentación correspondiente al proyecto de **Tesis de Grado** de tres estudiantes de Ingeniería Electrónica de la **Universidad Católica de Córdoba (UCC)**.

La finalidad principal del proyecto es el diseño, construcción y validación de un **Clinostato 3D o Máquina de Posicionamiento Aleatorio (RPM) con 2 ejes de libertad**. Este dispositivo electromecánico tiene como objetivo simular condiciones de microgravedad en la Tierra para su uso en la investigación de sistemas biológicos (cultivos celulares, plantas, bacterias, etc.).

## Fundamento Teórico
Dado que es extremadamente costoso y logísticamente complejo realizar experimentos en el espacio (como en la Estación Espacial Internacional), los clinostatos 3D y las RPM se presentan como alternativas viables en laboratorios terrestres. 

El dispositivo consta de dos marcos perpendiculares (uno interno y otro externo) impulsados por motores independientes. Al colocar una muestra biológica en el centro de rotación y alterar continuamente su orientación en los ejes, el vector de la aceleración gravitacional cambia de dirección de manera constante y más rápida que el tiempo de respuesta biológica del organismo en estudio. 

Como resultado, la gravedad experimentada por la muestra se anula en todas las direcciones tridimensionales, logrando lo que se conoce como **Microgravedad Simulada Promediada en el Tiempo (taSMG)**. Se espera que el dispositivo logre alcanzar valores de gravedad residual en el orden de $10^{-3} g$ a $10^{-4} g$.

## Objetivos del Proyecto
*   **Diseño Mecánico:** Construcción de una estructura robusta (marcos interno y externo) capaz de soportar las muestras y rotar con fluidez evitando vibraciones parásitas.
*   **Electrónica y Control:** Desarrollo de la placa de control para sincronizar y manejar los motores (como servomotores o motores paso a paso) garantizando precisión en la velocidad angular.
*   **Algoritmos de Simulación:** Implementación de distintos modos de funcionamiento, incluyendo:
    *   *Modo Clinostato 3D:* Rotación a velocidades constantes en ambos marcos.
    *   *Modo RPM (Random Positioning Machine):* Rotación con velocidades y sentidos de giro aleatorios para asegurar una distribución esférica homogénea del vector de gravedad (evitando el sesgo de polos y la predictibilidad del movimiento).
*   **Validación:** Uso de sensores inerciales (IMUs) en el centro de rotación para registrar los datos cinemáticos y corroborar matemáticamente que el promedio temporal de la gravedad tiende a cero.

## Estructura del Repositorio
*Por definir a medida que avance el desarrollo.* El repositorio albergará las carpetas para el código embebido (firmware), archivos de diseño CAD, diagramas de circuitos y scripts de análisis de datos.

## Contribución y Flujo de Trabajo
Este repositorio sigue un modelo de desarrollo estricto. **No se permite realizar merge directo a la rama principal**. Todo cambio debe realizarse mediante un *Pull Request* desde una rama de desarrollo y requiere la revisión y aprobación explícita de al menos uno de los integrantes del equipo antes de ser integrado.

Para más detalles sobre las convenciones de trabajo de este proyecto, por favor revisar el archivo [AGENTS.md](./AGENTS.md).