using Quarkit.Core.Manifest;
using Quarkit.Models.Core;

namespace Quarkit.Tests
{
    public class QkOptionResolverParsingTests
    {
        private readonly QkOptionResolver _resolver = new(new List<QkOptionDefinition>());

        #region Valid Test Cases

        [Test]
        [Arguments("option=value", "option", QkOperator.Assignment, "value")]
        [Arguments("option==value", "option", QkOperator.Equal, "value")]
        [Arguments("option<value", "option", QkOperator.Lesser, "value")]
        [Arguments("option<=value", "option", QkOperator.LesserEqual, "value")]
        [Arguments("option>value", "option", QkOperator.Greater, "value")]
        [Arguments("option>=value", "option", QkOperator.GreaterEqual, "value")]
        [Arguments("option!value", "option", QkOperator.IfNotNull, "value")]
        [Arguments("option!=value", "option", QkOperator.NotEqual, "value")]
        [Arguments("option?value", "option", QkOperator.IfNull, "value")]
        [Arguments("option?=value", "option", QkOperator.IfNullEqual, "value")]
        public async Task ParseExpression_ValidOperators_ShouldParseCorrectly(
            string expressionStr,
            string expectedOption,
            QkOperator expectedOperator,
            string expectedValue)
        {
            // Act
            var result = _resolver.Parse(expressionStr);

            // Assert
            await Assert.That(result).IsNotNull();
            await Assert.That(result!.Option).IsEqualTo(expectedOption);
            await Assert.That(result.Operator).IsEqualTo(expectedOperator);
            await Assert.That(result.Value).IsEqualTo(expectedValue);
        }

        [Test]
        [Arguments("exampleOption=\"testing\"", "exampleOption", QkOperator.Assignment, "\"testing\"")]
        [Arguments("complex_opt_123>=99.9", "complex_opt_123", QkOperator.GreaterEqual, "99.9")]
        public async Task ParseExpression_ComplexValues_ShouldPreserveQuotesAndFormats(
            string expressionStr,
            string expectedOption,
            QkOperator expectedOperator,
            string expectedValue)
        {
            // Act
            var result = _resolver.Parse(expressionStr);

            // Assert
            await Assert.That(result).IsNotNull();
            await Assert.That(result!.Option).IsEqualTo(expectedOption);
            await Assert.That(result.Operator).IsEqualTo(expectedOperator);
            await Assert.That(result.Value).IsEqualTo(expectedValue);
        }

        #endregion

        #region Edge Cases & Invalid Inputs

        [Test]
        [Arguments("")]
        [Arguments("no_operator_here")]
        [Arguments("just_letters_and_numbers_123")]
        public async Task ParseExpression_NoValidOperator_ShouldReturnNull(string expressionStr)
        {
            // Act
            var result = _resolver.Parse(expressionStr);

            // Assert
            await Assert.That(result).IsNull();
        }

        [Test]
        [Arguments("option=", "option", QkOperator.Assignment, "")]
        [Arguments("option<=", "option", QkOperator.LesserEqual, "")]
        public async Task ParseExpression_EmptyValue_ShouldReturnEmptyStringValue(
            string expressionStr,
            string expectedOption,
            QkOperator expectedOperator,
            string expectedValue)
        {
            // Act
            var result = _resolver.Parse(expressionStr);

            // Assert
            await Assert.That(result).IsNotNull();
            await Assert.That(result!.Option).IsEqualTo(expectedOption);
            await Assert.That(result.Operator).IsEqualTo(expectedOperator);
            await Assert.That(result.Value).IsEqualTo(expectedValue);
        }

        [Test]
        [Arguments("=value", "", QkOperator.Assignment, "value")]
        [Arguments("!=value", "", QkOperator.NotEqual, "value")]
        public async Task ParseExpression_EmptyOption_ShouldReturnEmptyStringOption(
            string expressionStr,
            string expectedOption,
            QkOperator expectedOperator,
            string expectedValue)
        {
            // Act
            var result = _resolver.Parse(expressionStr);

            // Assert
            await Assert.That(result).IsNotNull();
            await Assert.That(result!.Option).IsEqualTo(expectedOption);
            await Assert.That(result.Operator).IsEqualTo(expectedOperator);
            await Assert.That(result.Value).IsEqualTo(expectedValue);
        }

        #endregion
    }
}
