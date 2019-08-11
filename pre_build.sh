#!/usr/bin/env bash
PROJECT_DIR="$(pwd)"
WORK_DIR="${PROJECT_DIR}/tmp"
[[ -d ${WORK_DIR} ]] || mkdir ${WORK_DIR}
cd ${WORK_DIR}

# Скачиваем файл с темой если его нет
REMOTE_ZIP_FILE="https://github.com/dhg/Skeleton/releases/download/2.0.4/Skeleton-2.0.4.zip"
LOCAL_ZIP_FILE="${WORK_DIR}/$(basename ${REMOTE_ZIP_FILE})"
LOCAL_ZIP_FILE_BASE=$(basename ${LOCAL_ZIP_FILE})
[[ -f ${LOCAL_ZIP_FILE} ]] || wget -q ${REMOTE_ZIP_FILE}

# Распаковываем тему
CSS_DIRECTORY="${WORK_DIR}/${LOCAL_ZIP_FILE_BASE%.*}/css"
[[ -d ${CSS_DIRECTORY} ]] || unzip ${LOCAL_ZIP_FILE}

# Качаем тул минификации
REMOTE_TOOL_FILE="https://github.com/tdewolff/minify/releases/download/v2.5.1/minify_2.5.1_darwin_amd64.tar.gz"
[[ -f ${REMOTE_TOOL_FILE} ]] || wget -q ${REMOTE_TOOL_FILE}

# Распаковываем тул минификации
[[ -f "${WORK_DIR}/minify" ]] || tar -xzvf $(basename ${REMOTE_TOOL_FILE})
chmod +x "${WORK_DIR}/minify"

# Минифицируем и склеиваем css
./minify -o skeleton.min.css \
    "${CSS_DIRECTORY}/normalize.css" \
    "${CSS_DIRECTORY}/skeleton.css"

# Сжимаем css и перемещаем в ресурсы
tar -czvf skeleton.min.css.tar.gz skeleton.min.css
cd ${PROJECT_DIR}
cp ${WORK_DIR}/skeleton.min.css.tar.gz resources/skeleton.min.css.tar.gz

# Генерируем байт массив для архива с темой
xxd -i resources/skeleton.min.css.tar.gz include/skeleton.inl

# Генерируем массив байт для иконки
xxd -i resources/favicon.png include/favicon.inl

# Генерируем массив байт для index.html страницы
xxd -i resources/index.html include/index.html.inl

rm -rf ${WORK_DIR}