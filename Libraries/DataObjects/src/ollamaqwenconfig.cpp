#include "ollamaqwenconfig.hpp"

namespace DataObjects {

OllamaQwenConfig::OllamaQwenConfig() {
    setModel("qwen3.5");
    setSystemPrompt("You are a helpful assistant.");
    setTemperature(0.7f);
    setTopP(0.8f);
    setTopK(20);
    setPresencePenalty(1.5f);
    setNumCtx(32768);
    setTemplateStr(R"({{ if .System }}<|im_start|>system
{{ .System }}<|im_end|>
{{ end }}{{ if .Prompt }}<|im_start|>user
{{ .Prompt }}<|im_end|>
{{ end }}<|im_start|>assistant
)");
}

void OllamaQwenConfig::enableThinkingMode(bool enable) {
    if (enable) {
        setTemperature(1.0f);
        setTopP(0.95f);
    } else {
        setTemperature(0.7f);
        setTopP(0.8f);
    }
}

} // namespace DataObjects
