#include "airequest.hpp"

#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace DataObjects {

const std::string &AIRequest::model() const { return m_model; }

void AIRequest::setModel(const std::string &value) { m_model = value; }

const std::string &AIRequest::prompt() const { return m_prompt; }

const std::optional<std::string> &AIRequest::suffix() const { return m_suffix; }

void AIRequest::setSuffix(const std::optional<std::string> &value) { m_suffix = value; }

const std::vector<std::string> &AIRequest::images() const { return m_images; }

void AIRequest::setImages(const std::vector<std::string> &value) { m_images = value; }

const std::optional<std::string> &AIRequest::format() const { return m_format; }

void AIRequest::setFormat(const std::optional<std::string> &value) { m_format = value; }

const std::optional<std::string> &AIRequest::system() const { return m_system; }

void AIRequest::setSystem(const std::optional<std::string> &value) { m_system = value; }

const std::optional<std::string> &AIRequest::templateStr() const { return m_template; }

void AIRequest::setTemplateStr(const std::optional<std::string> &value) { m_template = value; }

const std::optional<std::vector<int> > &AIRequest::context() const { return m_context; }

void AIRequest::setContext(const std::optional<std::vector<int> > &value) { m_context = value; }

std::optional<bool> AIRequest::stream() const { return m_stream; }

void AIRequest::setStream(std::optional<bool> value) { m_stream = value; }

std::optional<bool> AIRequest::raw() const { return m_raw; }

void AIRequest::setRaw(std::optional<bool> value) { m_raw = value; }

const std::optional<std::variant<int, std::string> > &AIRequest::keepAlive() const { return m_keepAlive; }

void AIRequest::setKeepAlive(const std::optional<std::variant<int, std::string> > &value) { m_keepAlive = value; }

std::optional<int> AIRequest::numPredict() const { return m_numPredict; }

void AIRequest::setNumPredict(std::optional<int> value) { m_numPredict = value; }

std::optional<float> AIRequest::temperature() const { return m_temperature; }

void AIRequest::setTemperature(std::optional<float> value) { m_temperature = value; }

std::optional<int> AIRequest::topK() const { return m_topK; }

void AIRequest::setTopK(std::optional<int> value) { m_topK = value; }

std::optional<float> AIRequest::topP() const { return m_topP; }

void AIRequest::setTopP(std::optional<float> value) { m_topP = value; }

std::optional<int64_t> AIRequest::seed() const { return m_seed; }

void AIRequest::setSeed(std::optional<int64_t> value) { m_seed = value; }

const std::optional<std::vector<std::string> > &AIRequest::stop() const { return m_stop; }

void AIRequest::setStop(const std::optional<std::vector<std::string> > &value) { m_stop = value; }

std::optional<int> AIRequest::numCtx() const { return m_numCtx; }

void AIRequest::setNumCtx(std::optional<int> value) { m_numCtx = value; }

std::optional<float> AIRequest::repeatPenalty() const { return m_repeatPenalty; }

void AIRequest::setRepeatPenalty(std::optional<float> value) { m_repeatPenalty = value; }

std::optional<int> AIRequest::repeatLastN() const { return m_repeatLastN; }

void AIRequest::setRepeatLastN(std::optional<int> value) { m_repeatLastN = value; }

std::optional<int> AIRequest::numGpu() const { return m_numGpu; }

void AIRequest::setNumGpu(std::optional<int> value) { m_numGpu = value; }

std::optional<int> AIRequest::numThread() const { return m_numThread; }

void AIRequest::setNumThread(std::optional<int> value) { m_numThread = value; }

std::string AIRequest::toJson() const
{
    nlohmann::json j;

    // Common settings
    j["model"] = m_model;
    j["prompt"] = m_prompt;
    if (m_suffix) j["suffix"] = *m_suffix;
    if (!m_images.empty()) j["images"] = m_images;
    if (m_format) j["format"] = *m_format;
    if (m_system) j["system"] = *m_system;
    if (m_template) j["template"] = *m_template;
    if (m_context) j["context"] = *m_context;
    if (m_stream) j["stream"] = *m_stream;
    if (m_raw) j["raw"] = *m_raw;
    if (m_keepAlive) {
        std::visit([&j](auto&& val) { j["keep_alive"] = val; }, *m_keepAlive);
    }

    // Options
    nlohmann::json opts;
    if (m_numPredict) opts["num_predict"] = *m_numPredict;
    if (m_temperature) opts["temperature"] = *m_temperature;
    if (m_topK) opts["top_k"] = *m_topK;
    if (m_topP) opts["top_p"] = *m_topP;
    if (m_seed) opts["seed"] = *m_seed;
    if (m_stop) opts["stop"] = *m_stop;
    if (m_numCtx) opts["num_ctx"] = *m_numCtx;
    if (m_repeatPenalty) opts["repeat_penalty"] = *m_repeatPenalty;
    if (m_repeatLastN) opts["repeat_last_n"] = *m_repeatLastN;
    if (m_numGpu) opts["num_gpu"] = *m_numGpu;
    if (m_numThread) opts["num_thread"] = *m_numThread;

    if (!opts.empty()) {
        j["options"] = opts;
    }

    return j.dump();
}

bool AIRequest::readJson(const std::string &iString)
{
    if (iString.size() < 2) {
        COMPLOG_ERROR("[AIRequest] Parsing error: empty input");
        return false;
    }
    try {
        nlohmann::json j = nlohmann::json::parse(iString);

        if (j.contains("model")) m_model = j["model"].get<std::string>();
        if (j.contains("prompt")) m_prompt = j["prompt"].get<std::string>();

        if (j.contains("suffix")) m_suffix = j["suffix"].get<std::string>();
        if (j.contains("format")) m_format = j["format"].get<std::string>();
        if (j.contains("system")) m_system = j["system"].get<std::string>();
        if (j.contains("template")) m_template = j["template"].get<std::string>();
        if (j.contains("stream")) m_stream = j["stream"].get<bool>();
        if (j.contains("raw")) m_raw = j["raw"].get<bool>();

        // images — массив строк base64
        if (j.contains("images") && j["images"].is_array()) {
            m_images.clear();
            for (const auto& img : j["images"]) {
                m_images.push_back(img.get<std::string>());
            }
        }

        // context — массив чисел
        if (j.contains("context") && j["context"].is_array()) {
            m_context = j["context"].get<std::vector<int>>();
        }

        // keep_alive — может быть числом или строкой
        if (j.contains("keep_alive")) {
            const auto& ka = j["keep_alive"];
            if (ka.is_number_integer()) {
                m_keepAlive = ka.get<int>();
            } else if (ka.is_string()) {
                m_keepAlive = ka.get<std::string>();
            }
        }

        if (j.contains("options") && j["options"].is_object()) {
            const auto& opts = j["options"];
            if (opts.contains("num_predict")) m_numPredict = opts["num_predict"].get<int>();
            if (opts.contains("temperature")) m_temperature = opts["temperature"].get<float>();
            if (opts.contains("top_k")) m_topK = opts["top_k"].get<int>();
            if (opts.contains("top_p")) m_topP = opts["top_p"].get<float>();
            if (opts.contains("seed")) m_seed = opts["seed"].get<int64_t>();
            if (opts.contains("num_ctx")) m_numCtx = opts["num_ctx"].get<int>();
            if (opts.contains("repeat_penalty")) m_repeatPenalty = opts["repeat_penalty"].get<float>();
            if (opts.contains("repeat_last_n")) m_repeatLastN = opts["repeat_last_n"].get<int>();
            if (opts.contains("num_gpu")) m_numGpu = opts["num_gpu"].get<int>();
            if (opts.contains("num_thread")) m_numThread = opts["num_thread"].get<int>();

            // stop — массив строк
            if (opts.contains("stop") && opts["stop"].is_array()) {
                m_stop = opts["stop"].get<std::vector<std::string>>();
            }
        }

        return true;
    } catch (const nlohmann::json::exception& ex) {
        COMPLOG_ERROR("[AIRequest] Parsing error:", ex.what());
    }
    return false;
}

void AIRequest::setRequest(const std::string &request)
{
    m_prompt = request;

    // Prepare request
    boost::algorithm::replace_all(m_prompt, "\"", "\\\"");
    boost::algorithm::replace_all(m_prompt, "\n", "\\n");
}

} // namespace DataObjects
