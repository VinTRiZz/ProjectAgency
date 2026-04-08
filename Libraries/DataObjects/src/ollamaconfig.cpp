#include "ollamaconfig.hpp"

#include <sstream>
#include <regex>
#include <stdexcept>

namespace DataObjects {

// ---------- Геттеры / сеттеры (однострочные) ----------
const std::string& OllamaConfig::model() const { return m_model; }
void OllamaConfig::setModel(const std::string& value) { m_model = value; }

const std::string& OllamaConfig::systemPrompt() const { return m_systemPrompt; }
void OllamaConfig::setSystemPrompt(const std::string& value) { m_systemPrompt = value; }

const std::string& OllamaConfig::templateStr() const { return m_template; }
void OllamaConfig::setTemplateStr(const std::string& value) { m_template = value; }

const std::vector<std::string>& OllamaConfig::stop() const { return m_stop; }
void OllamaConfig::setStop(const std::vector<std::string>& value) { m_stop = value; }

int64_t OllamaConfig::seed() const { return m_seed; }
void OllamaConfig::setSeed(int64_t value) { m_seed = value; }

int32_t OllamaConfig::numPredict() const { return m_numPredict; }
void OllamaConfig::setNumPredict(int32_t value) { m_numPredict = value; }

int32_t OllamaConfig::numCtx() const { return m_numCtx; }
void OllamaConfig::setNumCtx(int32_t value) { m_numCtx = value; }

int32_t OllamaConfig::numKeep() const { return m_numKeep; }
void OllamaConfig::setNumKeep(int32_t value) { m_numKeep = value; }

int32_t OllamaConfig::numBatch() const { return m_numBatch; }
void OllamaConfig::setNumBatch(int32_t value) { m_numBatch = value; }

int32_t OllamaConfig::numGpu() const { return m_numGpu; }
void OllamaConfig::setNumGpu(int32_t value) { m_numGpu = value; }

int32_t OllamaConfig::mainGpu() const { return m_mainGpu; }
void OllamaConfig::setMainGpu(int32_t value) { m_mainGpu = value; }

bool OllamaConfig::lowVram() const { return m_lowVram; }
void OllamaConfig::setLowVram(bool value) { m_lowVram = value; }

bool OllamaConfig::f16Kv() const { return m_f16Kv; }
void OllamaConfig::setF16Kv(bool value) { m_f16Kv = value; }

bool OllamaConfig::vocabOnly() const { return m_vocabOnly; }
void OllamaConfig::setVocabOnly(bool value) { m_vocabOnly = value; }

bool OllamaConfig::useMmap() const { return m_useMmap; }
void OllamaConfig::setUseMmap(bool value) { m_useMmap = value; }

bool OllamaConfig::useMlock() const { return m_useMlock; }
void OllamaConfig::setUseMlock(bool value) { m_useMlock = value; }

bool OllamaConfig::useNuma() const { return m_useNuma; }
void OllamaConfig::setUseNuma(bool value) { m_useNuma = value; }

int32_t OllamaConfig::numThread() const { return m_numThread; }
void OllamaConfig::setNumThread(int32_t value) { m_numThread = value; }

float OllamaConfig::temperature() const { return m_temperature; }
void OllamaConfig::setTemperature(float value) { m_temperature = value; }

float OllamaConfig::topP() const { return m_topP; }
void OllamaConfig::setTopP(float value) { m_topP = value; }

int32_t OllamaConfig::topK() const { return m_topK; }
void OllamaConfig::setTopK(int32_t value) { m_topK = value; }

float OllamaConfig::minP() const { return m_minP; }
void OllamaConfig::setMinP(float value) { m_minP = value; }

float OllamaConfig::typicalP() const { return m_typicalP; }
void OllamaConfig::setTypicalP(float value) { m_typicalP = value; }

float OllamaConfig::tfsZ() const { return m_tfsZ; }
void OllamaConfig::setTfsZ(float value) { m_tfsZ = value; }

float OllamaConfig::repeatPenalty() const { return m_repeatPenalty; }
void OllamaConfig::setRepeatPenalty(float value) { m_repeatPenalty = value; }

int32_t OllamaConfig::repeatLastN() const { return m_repeatLastN; }
void OllamaConfig::setRepeatLastN(int32_t value) { m_repeatLastN = value; }

float OllamaConfig::presencePenalty() const { return m_presencePenalty; }
void OllamaConfig::setPresencePenalty(float value) { m_presencePenalty = value; }

float OllamaConfig::frequencyPenalty() const { return m_frequencyPenalty; }
void OllamaConfig::setFrequencyPenalty(float value) { m_frequencyPenalty = value; }

bool OllamaConfig::penalizeNewline() const { return m_penalizeNewline; }
void OllamaConfig::setPenalizeNewline(bool value) { m_penalizeNewline = value; }

int32_t OllamaConfig::mirostat() const { return m_mirostat; }
void OllamaConfig::setMirostat(int32_t value) { m_mirostat = value; }

float OllamaConfig::mirostatTau() const { return m_mirostatTau; }
void OllamaConfig::setMirostatTau(float value) { m_mirostatTau = value; }

float OllamaConfig::mirostatEta() const { return m_mirostatEta; }
void OllamaConfig::setMirostatEta(float value) { m_mirostatEta = value; }

const std::string& OllamaConfig::format() const { return m_format; }
void OllamaConfig::setFormat(const std::string& value) { m_format = value; }

const std::optional<bool>& OllamaConfig::stream() const { return m_stream; }
void OllamaConfig::setStream(const std::optional<bool>& value) { m_stream = value; }

const std::optional<int32_t>& OllamaConfig::keepAlive() const { return m_keepAlive; }
void OllamaConfig::setKeepAlive(const std::optional<int32_t>& value) { m_keepAlive = value; }

const std::string& OllamaConfig::suffix() const { return m_suffix; }
void OllamaConfig::setSuffix(const std::string& value) { m_suffix = value; }

// ---------- Сериализация ----------
std::string OllamaConfig::toModelfileString() const {
    std::ostringstream oss;
    oss << "FROM " << m_model << "\n";
    if (!m_systemPrompt.empty()) oss << "SYSTEM \"\"\"" << m_systemPrompt << "\"\"\"\n";
    if (!m_template.empty()) oss << "TEMPLATE \"\"\"" << m_template << "\"\"\"\n";
    auto addParam = [&](const std::string& name, const auto& value) { oss << "PARAMETER " << name << " " << value << "\n"; };
    addParam("temperature", m_temperature);
    addParam("top_p", m_topP);
    addParam("top_k", m_topK);
    addParam("min_p", m_minP);
    addParam("typical_p", m_typicalP);
    addParam("tfs_z", m_tfsZ);
    addParam("repeat_penalty", m_repeatPenalty);
    addParam("repeat_last_n", m_repeatLastN);
    addParam("presence_penalty", m_presencePenalty);
    addParam("frequency_penalty", m_frequencyPenalty);
    addParam("mirostat", m_mirostat);
    addParam("mirostat_tau", m_mirostatTau);
    addParam("mirostat_eta", m_mirostatEta);
    addParam("seed", m_seed);
    addParam("num_predict", m_numPredict);
    addParam("num_ctx", m_numCtx);
    addParam("num_keep", m_numKeep);
    addParam("num_batch", m_numBatch);
    addParam("num_gpu", m_numGpu);
    addParam("main_gpu", m_mainGpu);
    oss << "PARAMETER penalize_newline " << (m_penalizeNewline ? "true" : "false") << "\n";
    oss << "PARAMETER low_vram " << (m_lowVram ? "true" : "false") << "\n";
    oss << "PARAMETER f16_kv " << (m_f16Kv ? "true" : "false") << "\n";
    oss << "PARAMETER vocab_only " << (m_vocabOnly ? "true" : "false") << "\n";
    oss << "PARAMETER use_mmap " << (m_useMmap ? "true" : "false") << "\n";
    oss << "PARAMETER use_mlock " << (m_useMlock ? "true" : "false") << "\n";
    oss << "PARAMETER use_numa " << (m_useNuma ? "true" : "false") << "\n";
    oss << "PARAMETER num_thread " << m_numThread << "\n";
    for (const auto& s : m_stop) oss << "PARAMETER stop \"" << s << "\"\n";
    return oss.str();
}

void OllamaConfig::fromModelfileString(const std::string& content) {
    std::regex fromRe(R"(FROM\s+(\S+))", std::regex::icase);
    std::regex systemRe(R"(SYSTEM\s+"""([\s\S]*?)"")", std::regex::icase);
    std::regex templateRe(R"(TEMPLATE\s+"""([\s\S]*?)"")", std::regex::icase);
    std::regex paramRe(R"(PARAMETER\s+(\S+)\s+(.+))", std::regex::icase);
    std::smatch match;
    if (std::regex_search(content, match, fromRe)) m_model = match[1];
    if (std::regex_search(content, match, systemRe)) m_systemPrompt = match[1];
    if (std::regex_search(content, match, templateRe)) m_template = match[1];
    for (auto it = std::sregex_iterator(content.begin(), content.end(), paramRe); it != std::sregex_iterator(); ++it) {
        std::string name = (*it)[1];
        std::string value = (*it)[2];
        if (value.front() == '"' && value.back() == '"') value = value.substr(1, value.length() - 2);
        if (name == "temperature") m_temperature = std::stof(value);
        else if (name == "top_p") m_topP = std::stof(value);
        else if (name == "top_k") m_topK = std::stoi(value);
        else if (name == "min_p") m_minP = std::stof(value);
        else if (name == "typical_p") m_typicalP = std::stof(value);
        else if (name == "tfs_z") m_tfsZ = std::stof(value);
        else if (name == "repeat_penalty") m_repeatPenalty = std::stof(value);
        else if (name == "repeat_last_n") m_repeatLastN = std::stoi(value);
        else if (name == "presence_penalty") m_presencePenalty = std::stof(value);
        else if (name == "frequency_penalty") m_frequencyPenalty = std::stof(value);
        else if (name == "mirostat") m_mirostat = std::stoi(value);
        else if (name == "mirostat_tau") m_mirostatTau = std::stof(value);
        else if (name == "mirostat_eta") m_mirostatEta = std::stof(value);
        else if (name == "seed") m_seed = std::stoll(value);
        else if (name == "num_predict") m_numPredict = std::stoi(value);
        else if (name == "num_ctx") m_numCtx = std::stoi(value);
        else if (name == "num_keep") m_numKeep = std::stoi(value);
        else if (name == "num_batch") m_numBatch = std::stoi(value);
        else if (name == "num_gpu") m_numGpu = std::stoi(value);
        else if (name == "main_gpu") m_mainGpu = std::stoi(value);
        else if (name == "penalize_newline") m_penalizeNewline = (value == "true");
        else if (name == "low_vram") m_lowVram = (value == "true");
        else if (name == "f16_kv") m_f16Kv = (value == "true");
        else if (name == "vocab_only") m_vocabOnly = (value == "true");
        else if (name == "use_mmap") m_useMmap = (value == "true");
        else if (name == "use_mlock") m_useMlock = (value == "true");
        else if (name == "use_numa") m_useNuma = (value == "true");
        else if (name == "num_thread") m_numThread = std::stoi(value);
        else if (name == "stop") m_stop.push_back(value);
    }
}

}