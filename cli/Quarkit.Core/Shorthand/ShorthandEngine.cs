using System.Text;

namespace Quarkit.Core.Shorthand
{
    /// <summary>
    /// Handles registry of tokens and expanding of tokens to strings with the values in the tokens.
    /// 
    /// <para>
    /// Some Rules:
    /// </para>
    /// <list type="bullet">
    ///     <item>A token may not contain spaces</item>
    ///     <item>Escaping of tags is supported with \</item>
    ///     <item>When an opening tag is not closed it will be treated as a normal string</item>
    ///     <item>When an opening tag is not closed and another opening tag is found the previous tag is skipped.</item>
    ///     <item>When a token value is not found the token will not be expanded, and will remain in the expanded text.</item>
    /// </list>
    /// </summary>
    public class ShorthandEngine
    {
        /// <summary>
        /// Amount of tags required to create a token.
        /// </summary>
        public const int TAG_TYPES = 2;

        /// <summary>
        /// The character defining the opening of a token.
        /// </summary>
        public const char OPENING_TAG = '<';

        /// <summary>
        /// The characted defining the closure of a token.
        /// </summary>
        public const char CLOSING_TAG = '>';

        public readonly Dictionary<string, string> tokensAndValues = new(StringComparer.OrdinalIgnoreCase);

        /// <summary>
        /// Sets a token and the value related to it.
        /// </summary>
        /// <remarks>
        /// Rules: Must not contain spaces ' ', The opening and closing tags can appear but have to be escaped with '\'
        /// </remarks>
        /// <param name="key">Token key.</param>
        /// <param name="value">Value of the token.</param>
        public void SetToken(string key, string value)
        {
            string normalizedKey = key.StartsWith(OPENING_TAG) && key.EndsWith(CLOSING_TAG) ? key : $"{OPENING_TAG}{key}{CLOSING_TAG}";
            tokensAndValues[normalizedKey] = value; // Overwrites or sets
        }
       
        /// <summary>
        /// Expands tokens in <paramref name="input"/> from the values in <paramref name="contextualTokens"/> or global token registry if any exist.
        /// </summary>
        /// <param name="input">Input with possible tokens</param>
        /// <param name="contextualTokens">Contextual token registry (key is tokens)</param>
        /// <returns>A string with values expanded from tokens.</returns>
        public string Expand(string input, Dictionary<string, string>? contextualTokens = null)
        {
            if (string.IsNullOrEmpty(input)) return input;

            ReadOnlySpan<char> text = input.AsSpan();
            StringBuilder output = new StringBuilder(input.Length);

            bool isEscaped = false;
            bool inTag = false;
            int tagStartIdx = -1;

            for (int i = 0; i < text.Length; i++)
            {
                char c = text[i];

                if (isEscaped)
                {
                    output.Append(c);
                    isEscaped = false;
                    continue;
                }

                if (c == '\\')
                {
                    isEscaped = true;
                    continue;
                }

                if (c == OPENING_TAG)
                {
                    if (inTag)
                    {
                        output.Append(text.Slice(tagStartIdx, i - tagStartIdx));
                    }
                    inTag = true;
                    tagStartIdx = i;
                    continue;
                }

                if (c == CLOSING_TAG && inTag)
                {
                    int tagLength = i - tagStartIdx + 1;
                    ReadOnlySpan<char> tagSpan = text.Slice(tagStartIdx, tagLength);

                    // Rule: No spaces allowed in tags. This is checks if the string inside the tag contains a space.
                    if (tagSpan.Slice(1, tagLength - TAG_TYPES).Contains(' '))
                    {
                        output.Append(tagSpan); // Treats it as a normal string.
                    }
                    else
                    {
                        string tagStr = tagSpan.ToString();

                        string? resolvedValue = null;
                        if (contextualTokens != null && contextualTokens.TryGetValue(tagStr, out var ctxVal)) // Rule: Contextual tokens have higher priority
                        {
                            resolvedValue = ctxVal;
                        }
                        else if (tokensAndValues.TryGetValue(tagStr, out var globalVal))
                        {
                            resolvedValue = globalVal;
                        }

                        output.Append(resolvedValue ?? tagStr);
                    }

                    inTag = false;
                    continue;
                }

                if (!inTag)
                {
                    output.Append(c);
                }
            }

            // Handles a case where the opening tag was declared but the string ends without a closing tag.
            if (inTag)
            {
                output.Append(text.Slice(tagStartIdx).ToString());
            }

            return output.ToString();
        }
    }
}
