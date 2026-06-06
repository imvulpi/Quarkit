using Quarkit.Models.Core;
using System.Runtime.CompilerServices;

namespace Quarkit.Core.Manifest
{
    public class QkOptionResolver
    {
        public QkOptionResolver(Dictionary<string, QkOptionDefinition> definitions)
        {
            OptionDefinitions = definitions;
        }

        public QkOptionResolver(List<QkOptionDefinition> definitions)
        {
            OptionDefinitions = new(definitions.Count);
            foreach (var definition in definitions)
            {
                OptionDefinitions.Add(definition.Name, definition);
            }
        }

        public Dictionary<string, QkOptionDefinition> OptionDefinitions { get; set; }
        public Dictionary<string, string> OptionAndValues = new();

        public string[] BooleanValues = [
            "true", "false"
        ];

        public bool ExpressionsMatch(List<string> expressions)
        {
            for (int i = 0; i < expressions.Count; i++)
            {
                var expression = expressions[i];
                QkOptionExpression? parsedExpresion = Parse(expression);
                if (parsedExpresion == null) continue;

                if (OptionDefinitions.TryGetValue(parsedExpresion.Option, out var optionDefinition))
                {
                    if (!ResolveExpression(parsedExpresion, optionDefinition)) return false;
                }
                else
                {
                    throw new InvalidOperationException($"Unknown option used in an expression: {parsedExpresion.Option}");
                }
            }
            return true;
        }

        public bool ResolveExpression(QkOptionExpression expression, QkOptionDefinition definition)
        {
            return definition.Type switch
            {
                QkType.Boolean => ResolveBoolOperation(expression.Option, expression.Operator, expression.Value),
                QkType.Number => ResolveNumberOperation(expression.Option, expression.Operator, expression.Value),
                QkType.String => ResolveStringOperation(expression.Option, expression.Operator, expression.Value),
                _ => false,
            };
        }

        private bool ResolveStringOperation(string option, QkOperator qkOperator, string remoteValue)
        {
            OptionAndValues.TryGetValue(option, out string? localValue);
            switch (qkOperator)
            {
                case QkOperator.Assignment:
                _assignment:
                    OptionAndValues[option] = remoteValue;
                    return true;
                case QkOperator.Equal: // No nulls.
                    return remoteValue == localValue;
                case QkOperator.NotEqual:
                    return remoteValue != localValue;
                case QkOperator.IfNull: // Returns true if value is null
                    if (localValue == null) return true;
                    return false;
                case QkOperator.IfNotNull: // Returns true if there is value
                    if (localValue != null) return true;
                    return false;
                case QkOperator.IfNullEqual: // Sets if the local value is null
                    if (localValue == null) goto _assignment;
                    return true; // Always true
                default: return false;
            }
        }

        private bool ResolveNumberOperation(string option, QkOperator qkOperator, string remoteValue)
        {
            OptionAndValues.TryGetValue(option, out string? localValue);
            bool hasValue = localValue != null;
            hasValue = double.TryParse(localValue, out double localDouble);

            switch (qkOperator)
            {
                case QkOperator.Assignment:
                _assignment:
                    {
                        string lowerRemoteValue = remoteValue.ToLower();
                        if (lowerRemoteValue == "null")
                        {
                            OptionAndValues.Remove(option);
                            return true;
                        }
                        if (Double.TryParse(remoteValue, out double remoteDouble))
                            OptionAndValues[option] = remoteDouble.ToString(); // This ensures the value is always full double form (no scientific etc).
                        else throw new InvalidOperationException($"Cannot parse numeric value as double in Quarkit option: {option}");
                        return true;
                    }
                case QkOperator.Equal: // Parses to avoid problems with scientific notations etc.
                    if (localValue == null && remoteValue.ToLower() == "null") return true;
                    else if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && remoteDouble == localDouble) return true;
                    return false;
                case QkOperator.NotEqual:
                    if (localValue == null && remoteValue.ToLower() == "null") return false;
                    else if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && remoteDouble == localDouble) return false;
                    return true;
                case QkOperator.IfNull: // Returns true if value is null
                    if (localValue == null) return true;
                    return false;
                case QkOperator.IfNotNull: // Returns true if there is value
                    if (localValue != null) return true;
                    return false;
                case QkOperator.IfNullEqual: // Sets if the local value is null
                    if (localValue == null) goto _assignment;
                    return true; // Always true
                case QkOperator.Greater:
                    {
                        // This is correct because the option is always on the left.
                        if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && localDouble > remoteDouble) return true;
                        return false;
                    }
                case QkOperator.GreaterEqual:
                    {
                        // This is correct because the option is always on the left.
                        if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && localDouble >= remoteDouble) return true;
                        return false;
                    }
                case QkOperator.Lesser:
                    {
                        if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && localDouble < remoteDouble) return true;
                        return false;
                    }
                case QkOperator.LesserEqual:
                    {
                        if (hasValue && double.TryParse(remoteValue, out double remoteDouble) && localDouble <= remoteDouble) return true;
                        return false;
                    }
                default: return false;
            }
        }

        private bool ResolveBoolOperation(string option, QkOperator qkOperator, string remoteValue)
        {
            OptionAndValues.TryGetValue(option, out string? localValue);
            switch (qkOperator)
            {
                case QkOperator.Assignment:
                _assignment:
                    string lowerRemoteValue = remoteValue.ToLower();
                    if (lowerRemoteValue == "null")
                    {
                        OptionAndValues.Remove(option);
                        return true;
                    }
                    if (BooleanValues.Contains(lowerRemoteValue))
                        OptionAndValues[option] = remoteValue;
                    else throw new InvalidOperationException($"Unknown operator on Quarkit Boolean option ({option})");
                    return true;
                case QkOperator.Equal:
                    if (localValue == null && remoteValue.ToLower() == "null") return true;
                    else if (localValue != null && remoteValue.ToLower() == localValue) return true;
                    return false;
                case QkOperator.NotEqual:
                    if (localValue == null && remoteValue.ToLower() == "null") return false;
                    else if (localValue != null && remoteValue.ToLower() == localValue) return false;
                    return true;
                case QkOperator.IfNull: // Returns true if value is null
                    if (localValue == null) return true;
                    return false;
                case QkOperator.IfNotNull: // Returns true if there is value
                    if (localValue != null) return true;
                    return false;
                case QkOperator.IfNullEqual: // Sets if the local value is null
                    if (localValue == null) goto _assignment;
                    return true; // Always true
                default: return false;
            }
        }

        public QkOptionExpression? Parse(string expression)
        {
            int valueIndex = 0;
            QkOperator? qkOperator = null;

            ReadOnlySpan<char> expressionChars = expression.AsSpan();
            int index;
            for (index = 0; index < expressionChars.Length; index++)
            {
                char c = expressionChars[index];
                switch (c)
                {
                    case '=':
                        qkOperator = GetOperator(expressionChars, QkOperator.Assignment, QkOperator.Equal, index, ref valueIndex);
                        goto _resolved;
                    case '<':
                        qkOperator = GetOperator(expressionChars, QkOperator.Lesser, QkOperator.LesserEqual, index, ref valueIndex);
                        goto _resolved;
                    case '>':
                        qkOperator = GetOperator(expressionChars, QkOperator.Greater, QkOperator.GreaterEqual, index, ref valueIndex);
                        goto _resolved;
                    case '!':
                        qkOperator = GetOperator(expressionChars, QkOperator.IfNotNull, QkOperator.NotEqual, index, ref valueIndex);
                        goto _resolved;
                    case '?':
                        qkOperator = GetOperator(expressionChars, QkOperator.IfNull, QkOperator.IfNullEqual, index, ref valueIndex);
                        goto _resolved;
                }
            }

        _resolved:

            if (qkOperator != null)
            {
                return new()
                {
                    Operator = qkOperator.Value,
                    Option = expression.AsSpan(0, index).ToString(),
                    Value = expression.AsSpan(index + valueIndex).ToString()
                };
            }
            return null;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static QkOperator GetOperator(ReadOnlySpan<char> expression, QkOperator single, QkOperator nextMatch, int index, ref int valueIndex)
        {
            if (expression.Length > index + 1 && expression[index + 1] == '=')
            {
                valueIndex = 2;
                return nextMatch;
            }
            else
            {
                valueIndex = 1;
                return single;
            }
        }
    }
}
