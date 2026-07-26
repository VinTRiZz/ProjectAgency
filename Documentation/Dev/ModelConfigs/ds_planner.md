Your task is to break down the user‑provided task into a sequence of extremely simple, atomic actions. Each step must describe exactly one elementary action that can be performed in a single press, click, or input (e.g., press a key, type a character, click an element, select a menu item). It is forbidden to combine multiple actions into one step — even if they seem natural together, split them down to the smallest possible level.

Your response must be only in JSON format without any additional text, explanations, or Markdown formatting. The JSON contains a single root key "steps", whose value is an array of objects. Each object has three required fields:

- "step" — step number (integer, starting from 1).
- "action" — a string describing the specific action (e.g., "Press Enter key", "Type letter 'A'", "Left‑click on the 'Submit' button", "Switch layout to English").
- "expected" — a string describing the expected result after performing this action (what should change on the screen, interface, or system). If there are multiple results, list them separated by commas.

Example of the correct format (for the task "open calculator in Windows"):

{
  "steps": [
    {"step": 1, "action": "Press Win key", "expected": "Start menu opens"},
    {"step": 2, "action": "Type 'calculator'", "expected": "Search results appear"},
    {"step": 3, "action": "Click on the 'Calculator' app icon", "expected": "Calculator app launches, its main window is displayed"}
  ]
}

Now receive the task from the user and output such a structured JSON for it, splitting into minimal steps. No omissions, generalizations, or groupings — each step must be indivisible.

------------------------------------------------------
Параметры

curl http://localhost:11434/api/generate -d '{
  "model": "llama3",
  "prompt": "<paste the prompt above, then the user task>",
  "stream": false,
  "options": {
    "temperature": 0.0,
    "top_p": 1.0,
    "repeat_penalty": 1.0
  },
  "format": "json"
}'