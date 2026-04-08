#! /bin/bash

if (! test -e "$1"); then
    echo "File not exist: $1"
    exit 1
fi

if [ "" == "$2" ]; then
    echo "Invalid empty prompt"
    exit 2
fi

# Логфайл агента
# LOCAL_logfile="generation.log"
LOCAL_logfile="/dev/null"

{
echo "========================================"
echo "===> GENERATION DATE: $(date +%d.%m.%Y_%H-%M-%S)"
echo "========================================"
} >> $LOCAL_logfile

# Plan
echo "Составление плана решения..."
LOCAL_processPlan=$(cat $1 |
ollama run deepseek-coder-v2:16b "Ответь в формате только пунктов плана. Проанализируй этот файл и напиши план для реализации следующей задачи: '$2'")
{
echo "======================================== BEG PLAN"
echo "$LOCAL_processPlan"
echo "======================================== END PLAN"
} >> $LOCAL_logfile


# Test solution
echo "Генерация первичного решения..."
LOCAL_processTestIteration=$(cat $1 | ollama run deepseek-coder-v2:16b "Твоя задача - сделать решение, которое точно выполняет задачу. Твой ответ ОБЯЗАН состоять из блоков <test>, <analyse>, <solution>. Сначала сгенерируй тестовую версию в блоке <test>, потом в блоке <analyse> проверь, корректно ли ты реализовал решение. Твоя задача - гарантировать выполнение (пройди пошагово по каждой строчке решения). После анализа ответь в формате ТОЛЬКО кода в блоке <solution> с проверенным и исправленным, при необходимости, результатом. Не меняй тот код, который не касается задачи. Выполни задачу '$2' в соответствии с планом: $LOCAL_processPlan")
{
echo "======================================== BEG TEST ITERATION"
echo "$LOCAL_processTestIteration"
echo "======================================== END TEST ITERATION"
} >> $LOCAL_logfile


# DEBUG
echo "$LOCAL_processTestIteration"
