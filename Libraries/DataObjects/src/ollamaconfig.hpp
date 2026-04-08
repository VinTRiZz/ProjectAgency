#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace DataObjects
{

/**
 * @brief The OllamaConfig class Configuration of model in Ollama
 * @note on 08.04.2026 - AI-generated, so comments are on Russian
 */
class OllamaConfig {
public:
    virtual ~OllamaConfig() = default;

    // --- Геттеры / сеттеры основных параметров API ---
    const std::string& model() const;                     ///< Имя модели
    void setModel(const std::string& value);              ///< Установить имя модели

    const std::string& systemPrompt() const;              ///< Системный промпт
    void setSystemPrompt(const std::string& value);       ///< Установить системный промпт

    const std::string& templateStr() const;               ///< Шаблон сообщения (Go template)
    void setTemplateStr(const std::string& value);        ///< Установить шаблон

    const std::vector<std::string>& stop() const;         ///< Стоп-слова
    void setStop(const std::vector<std::string>& value);  ///< Установить стоп-слова

    int64_t seed() const;                                 ///< Зерно ГСЧ (-1 = случайное)
    void setSeed(int64_t value);                          ///< Установить зерно

    int32_t numPredict() const;                           ///< Макс. число генерируемых токенов (-1 = не ограничено)
    void setNumPredict(int32_t value);                    ///< Установить макс. число токенов

    int32_t numCtx() const;                               ///< Размер контекстного окна
    void setNumCtx(int32_t value);                        ///< Установить размер контекста

    int32_t numKeep() const;                              ///< Число токенов, сохраняемых при сдвиге контекста
    void setNumKeep(int32_t value);                       ///< Установить num_keep

    int32_t numBatch() const;                             ///< Размер батча для промпта
    void setNumBatch(int32_t value);                      ///< Установить размер батча

    int32_t numGpu() const;                               ///< Число используемых GPU (-1 = авто)
    void setNumGpu(int32_t value);                        ///< Установить число GPU

    int32_t mainGpu() const;                              ///< Индекс основного GPU
    void setMainGpu(int32_t value);                       ///< Установить индекс основного GPU

    bool lowVram() const;                                 ///< Режим экономии видеопамяти
    void setLowVram(bool value);                          ///< Включить/выключить Low VRAM

    bool f16Kv() const;                                   ///< Использовать float16 для KV-кэша
    void setF16Kv(bool value);                            ///< Установить f16_kv

    bool vocabOnly() const;                               ///< Загружать только словарь (без весов)
    void setVocabOnly(bool value);                        ///< Установить vocab_only

    bool useMmap() const;                                 ///< Использовать memory mapping
    void setUseMmap(bool value);                          ///< Установить use_mmap

    bool useMlock() const;                                ///< Заблокировать модель в ОЗУ
    void setUseMlock(bool value);                         ///< Установить use_mlock

    bool useNuma() const;                                 ///< Использовать NUMA
    void setUseNuma(bool value);                          ///< Установить use_numa

    int32_t numThread() const;                            ///< Число потоков CPU (0 = авто)
    void setNumThread(int32_t value);                     ///< Установить num_thread

    // --- Параметры сэмплирования ---
    float temperature() const;                            ///< Температура (чем выше, тем более случайный вывод)
    void setTemperature(float value);                     ///< Установить температуру

    float topP() const;                                   ///< Nucleus sampling
    void setTopP(float value);                            ///< Установить top_p

    int32_t topK() const;                                 ///< Top-K sampling
    void setTopK(int32_t value);                          ///< Установить top_k

    float minP() const;                                   ///< Min-P sampling
    void setMinP(float value);                            ///< Установить min_p

    float typicalP() const;                               ///< Typical sampling
    void setTypicalP(float value);                        ///< Установить typical_p

    float tfsZ() const;                                   ///< Tail Free Sampling
    void setTfsZ(float value);                            ///< Установить tfs_z

    // --- Параметры повторений ---
    float repeatPenalty() const;                          ///< Штраф за повтор токенов
    void setRepeatPenalty(float value);                   ///< Установить repeat_penalty

    int32_t repeatLastN() const;                          ///< Окно для учёта повторов
    void setRepeatLastN(int32_t value);                   ///< Установить repeat_last_n

    float presencePenalty() const;                        ///< Штраф за присутствие токена
    void setPresencePenalty(float value);                 ///< Установить presence_penalty

    float frequencyPenalty() const;                       ///< Штраф за частоту токена
    void setFrequencyPenalty(float value);                ///< Установить frequency_penalty

    bool penalizeNewline() const;                         ///< Применять штраф к символу новой строки
    void setPenalizeNewline(bool value);                  ///< Установить penalize_newline

    // --- Mirostat ---
    int32_t mirostat() const;                             ///< Режим Mirostat (0/1/2)
    void setMirostat(int32_t value);                      ///< Установить mirostat

    float mirostatTau() const;                            ///< Целевая энтропия для Mirostat
    void setMirostatTau(float value);                     ///< Установить mirostat_tau

    float mirostatEta() const;                            ///< Скорость обучения Mirostat
    void setMirostatEta(float value);                     ///< Установить mirostat_eta

    // --- Дополнительные настройки ---
    const std::string& format() const;                    ///< Формат вывода (json или схема)
    void setFormat(const std::string& value);             ///< Установить формат

    const std::optional<bool>& stream() const;            ///< Флаг потоковой передачи ответа
    void setStream(const std::optional<bool>& value);     ///< Установить stream

    const std::optional<int32_t>& keepAlive() const;      ///< Время удержания модели в памяти (сек)
    void setKeepAlive(const std::optional<int32_t>& value);///< Установить keep_alive

    const std::string& suffix() const;                    ///< Суффикс, добавляемый после генерации
    void setSuffix(const std::string& value);             ///< Установить суффикс

    // --- Сериализация ---
    virtual std::string toModelfileString() const;        ///< Преобразовать в текст Modelfile
    virtual void fromModelfileString(const std::string& content); ///< Загрузить из текста Modelfile

private:
    // --- Основные параметры API ---
    std::string m_model;
    std::string m_systemPrompt;
    std::string m_template;
    std::vector<std::string> m_stop;
    int64_t m_seed = -1;
    int32_t m_numPredict = -1;
    int32_t m_numCtx = 2048;
    int32_t m_numKeep = -1;
    int32_t m_numBatch = 512;
    int32_t m_numGpu = -1;
    int32_t m_mainGpu = 0;
    bool m_lowVram = false;
    bool m_f16Kv = true;
    bool m_vocabOnly = false;
    bool m_useMmap = true;
    bool m_useMlock = false;
    bool m_useNuma = false;
    int32_t m_numThread = 0;

    // --- Параметры сэмплирования ---
    float m_temperature = 0.8f;
    float m_topP = 0.9f;
    int32_t m_topK = 40;
    float m_minP = 0.05f;
    float m_typicalP = 1.0f;
    float m_tfsZ = 1.0f;

    // --- Параметры повторений ---
    float m_repeatPenalty = 1.1f;
    int32_t m_repeatLastN = 64;
    float m_presencePenalty = 0.0f;
    float m_frequencyPenalty = 0.0f;
    bool m_penalizeNewline = false;

    // --- Mirostat ---
    int32_t m_mirostat = 0;
    float m_mirostatTau = 5.0f;
    float m_mirostatEta = 0.1f;

    // --- Дополнительные настройки ---
    std::string m_format;
    std::optional<bool> m_stream;
    std::optional<int32_t> m_keepAlive;
    std::string m_suffix;
};

}