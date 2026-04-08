#include "ollamadeepseekconfig.hpp"

namespace DataObjects {

OllamaDeepSeekConfig::OllamaDeepSeekConfig() {
    setModel("deepseek-r1");
    setSystemPrompt("You are DeepSeek-R1, an AI assistant created by DeepSeek.");
    // Официально рекомендованные параметры
    setTemperature(0.6f);
    setTopP(0.95f);
    setTopK(40);
    // DeepSeek-R1 может генерировать длинные цепочки рассуждений
    setNumPredict(4096);
    setNumCtx(32768);
    // Специфичный шаблон
    setTemplateStr(R"({{ if .System }}{{ .System }}
{{ end }}User: {{ .Prompt }}
Assistant: )");
}

void OllamaDeepSeekConfig::setReasoningEffort(const std::string &level) {
    if (level == "high") {
        setNumPredict(8192);
        setTemperature(0.5f); // Более детерминированно для сложных рассуждений
    } else if (level == "medium") {
        setNumPredict(4096);
        setTemperature(0.6f);
    } else { // low
        setNumPredict(2048);
        setTemperature(0.7f);
    }
}

} // namespace DataObjects
