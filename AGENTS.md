# Instrucciones para Agentes de IA (AI Agent Guidelines)

Este archivo define las reglas estrictas de comportamiento, estilo y flujo de trabajo que cualquier agente de Inteligencia Artificial (y desarrollador humano) debe seguir al interactuar con este repositorio.

## 1. Idioma del Repositorio
**Toda la comunicación, documentación, comentarios de código y mensajes de commit DEBEN ser redactados estrictamente en Español.** Aunque el estándar de la industria suele ser el inglés, este es un proyecto de tesis de la Universidad Católica de Córdoba y requiere que todo su contenido esté localizado al español.

## 2. Flujo de Trabajo (Version Control Workflow)
El repositorio funciona bajo un modelo estricto de control de versiones basado en revisión por pares.
*   **Prohibición de Mergeo Directo:** Está absolutamente prohibido hacer `push` o mergear código directamente a la rama `main` (o `master`).
*   **Desarrollo en Ramas (Branches):** Cualquier nueva característica, corrección o documentación debe realizarse en una rama separada (ej. `caracteristica/control-motores`, `arreglo/sensor-giroscopio`).
*   **Pull Requests (PR):** Una vez finalizado el trabajo en la rama, se debe abrir un Pull Request hacia la rama `main`.
*   **Revisión Obligatoria:** El PR no puede ser mergeado hasta que cuente con la aprobación (Approve) de al menos **uno de los compañeros de equipo** (los otros estudiantes de la tesis).

## 3. Estándar de Commits
Adoptamos una versión en español de *Conventional Commits* para mantener un historial limpio, legible y estructurado. El formato obligatorio es:

`<tipo>[ámbito opcional]: <descripción en imperativo>`

### Tipos permitidos:
*   `caracteristica:` (Equivalente a *feat*) Para nuevas funcionalidades (ej. nuevo algoritmo de control RPM).
*   `arreglo:` (Equivalente a *fix*) Para solucionar errores o bugs en el código o hardware.
*   `docs:` Para cambios única y exclusivamente en la documentación (ej. actualizar el README).
*   `estilo:` Para cambios de formato que no afectan la lógica del código (espaciados, punto y coma, etc.).
*   `refactor:` Para reestructuración del código que no corrige errores ni añade características.
*   `prueba:` (Equivalente a *test*) Para añadir o modificar pruebas de los sistemas.
*   `tarea:` (Equivalente a *chore*) Para actualizaciones de dependencias, configuraciones del entorno de desarrollo, etc.

### Ejemplos de uso:
*   `caracteristica(motores): implementar algoritmo de caminata aleatoria para el marco interior`
*   `arreglo(sensores): corregir lectura de aceleración residual en el eje z`
*   `docs: agregar marco teórico sobre microgravedad al README`

**Regla para la descripción:** La descripción debe escribirse en minúsculas, no debe terminar con un punto y debe estar conjugada en tiempo imperativo (como si estuvieras dando una orden: "agregar", "cambiar", "corregir").