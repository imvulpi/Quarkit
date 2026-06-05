using Quarkit.Core.Manifest;
using Quarkit.Models.Core;

namespace Quarkit.Tests;
public class QkOptionResolverResolutionTests
{
    private QkOptionResolver _resolver = null!;

    // Assuming OptionAndValues is exposed, or initialized empty per test execution
    [Before(HookType.Test)]
    public void Setup()
    {
        _resolver = new QkOptionResolver(new List<QkOptionDefinition>());
        _resolver.OptionAndValues.Clear();
    }

    #region String Operations

    [Test]
    public async Task String_Assignment_ShouldUpdateLocalValueAndReturnTrue()
    {
        var expr = new QkOptionExpression { Option = "theme", Operator = QkOperator.Assignment, Value = "dark" };
        var def = new QkOptionDefinition { Name="theme", Type = QkType.String };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsTrue();
        await Assert.That(_resolver.OptionAndValues["theme"]).IsEqualTo("dark");
    }

    [Test]
    [Arguments("dark", "dark", QkOperator.Equal, true)]
    [Arguments("dark", "light", QkOperator.Equal, false)]
    [Arguments("dark", "light", QkOperator.NotEqual, true)]
    public async Task String_Comparisons_ShouldEvaluateCorrectly(
        string localValue, string remoteValue, QkOperator op, bool expectedResult)
    {
        _resolver.OptionAndValues["theme"] = localValue;
        var expr = new QkOptionExpression { Option = "theme", Operator = op, Value = remoteValue };
        var def = new QkOptionDefinition { Name="theme", Type = QkType.String };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsEqualTo(expectedResult);
    }

    [Test]
    [Arguments(null, QkOperator.IfNull, true)]
    [Arguments("populated", QkOperator.IfNull, false)]
    [Arguments("populated", QkOperator.IfNotNull, true)]
    [Arguments(null, QkOperator.IfNotNull, false)]
    public async Task String_NullChecks_ShouldEvaluateState(string? localValue, QkOperator op, bool expectedResult)
    {
        if (localValue != null) _resolver.OptionAndValues["theme"] = localValue;
        var expr = new QkOptionExpression { Option = "theme", Operator = op, Value = "ignored" };
        var def = new QkOptionDefinition { Name="theme", Type = QkType.String };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsEqualTo(expectedResult);
    }

    [Test]
    public async Task String_IfNullEqual_WhenNull_ShouldAssignAndReturnTrue()
    {
        // theme is missing/null in dictionary
        var expr = new QkOptionExpression { Option = "theme", Operator = QkOperator.IfNullEqual, Value = "fallback" };
        var def = new QkOptionDefinition { Name="theme", Type = QkType.String };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsTrue();
        await Assert.That(_resolver.OptionAndValues["theme"]).IsEqualTo("fallback");
    }

    #endregion

    #region Number Operations

    [Test]
    public async Task Number_Assignment_NullString_ShouldRemoveKey()
    {
        _resolver.OptionAndValues["timeout"] = "30";
        var expr = new QkOptionExpression { Option = "timeout", Operator = QkOperator.Assignment, Value = "null" };
        var def = new QkOptionDefinition { Name = "timeout", Type = QkType.Number };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsTrue();
        await Assert.That(_resolver.OptionAndValues.ContainsKey("timeout")).IsFalse();
    }

    [Test]
    public void Number_Assignment_InvalidDouble_ShouldThrowException()
    {
        var expr = new QkOptionExpression { Option = "timeout", Operator = QkOperator.Assignment, Value = "not_a_number" };
        var def = new QkOptionDefinition { Name = "timeout", Type = QkType.Number };

        Assert.Throws<InvalidOperationException>(() => _resolver.ResolveExpression(expr, def));
    }

    [Test]
    [Arguments("50", "100", QkOperator.Greater, true)]
    [Arguments("50", "50", QkOperator.GreaterEqual, true)]
    [Arguments("100", "50", QkOperator.Lesser, true)]
    [Arguments("50", "50", QkOperator.LesserEqual, true)]
    public async Task Number_Inequalities_ShouldEvaluateCorrectly(
        string localValue, string remoteValue, QkOperator op, bool expectedResult)
    {
        _resolver.OptionAndValues["timeout"] = localValue;
        var expr = new QkOptionExpression { Option = "timeout", Operator = op, Value = remoteValue };
        var def = new QkOptionDefinition { Name = "timeout", Type = QkType.Number };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsEqualTo(expectedResult);
    }

    #endregion

    #region Boolean Operations

    [Test]
    [Arguments("true")]
    [Arguments("false")]
    public async Task Boolean_Assignment_ValidValue_ShouldSucceed(string remoteValue)
    {
        var expr = new QkOptionExpression { Option = "enabled", Operator = QkOperator.Assignment, Value = remoteValue };
        var def = new QkOptionDefinition { Name="enabled", Type = QkType.Boolean };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsTrue();
        await Assert.That(_resolver.OptionAndValues["enabled"]).IsEqualTo(remoteValue);
    }

    [Test]
    public void Boolean_Assignment_InvalidValue_ShouldThrowException()
    {
        var expr = new QkOptionExpression { Option = "enabled", Operator = QkOperator.Assignment, Value = "maybe" };
        var def = new QkOptionDefinition { Name= "enabled", Type = QkType.Boolean };

        Assert.Throws<InvalidOperationException>(() => _resolver.ResolveExpression(expr, def));
    }

    [Test]
    [Arguments("true", "true", QkOperator.Equal, true)]
    [Arguments("true", "false", QkOperator.Equal, false)]
    [Arguments(null, "null", QkOperator.Equal, true)]
    [Arguments("true", "null", QkOperator.NotEqual, true)]
    public async Task Boolean_Comparisons_ShouldEvaluateCorrectly(
        string? localValue, string remoteValue, QkOperator op, bool expectedResult)
    {
        if (localValue != null) _resolver.OptionAndValues["enabled"] = localValue;
        var expr = new QkOptionExpression { Option = "enabled", Operator = op, Value = remoteValue };
        var def = new QkOptionDefinition { Name= "enabled", Type = QkType.Boolean };

        var result = _resolver.ResolveExpression(expr, def);

        await Assert.That(result).IsEqualTo(expectedResult);
    }

    #endregion
}
