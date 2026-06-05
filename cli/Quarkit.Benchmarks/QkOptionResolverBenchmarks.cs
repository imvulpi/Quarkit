using BenchmarkDotNet.Attributes;
using BenchmarkDotNet.Order;
using Quarkit.Core.Manifest;
using Quarkit.Models.Core;
using System.Collections.Generic;

namespace Quarkit.Benchmarks
{
    [MemoryDiagnoser]
    [Orderer(SummaryOrderPolicy.FastestToSlowest)]
    [RankColumn]
    public class QkOptionResolverBenchmarks
    {
        private QkOptionResolver _resolver = null!;
        private QkOptionDefinition _stringDef = null!;
        private QkOptionDefinition _numberDef = null!;
        private QkOptionDefinition _boolDef = null!;

        private QkOptionExpression _stringExpr = null!;
        private QkOptionExpression _numberExpr = null!;
        private QkOptionExpression _boolExpr = null!;

        // Prevents compiler folding
        private string _simpleExpression = "exampleOption=\"testing\"";
        private string _complexExpression = "complex_opt_123>=99.9";
        private string _noOperatorExpression = "just_a_plain_string_with_no_operators_at_all";

        [GlobalSetup]
        public void Setup()
        {
            _resolver = new QkOptionResolver(new List<QkOptionDefinition>());

            _resolver.OptionAndValues["theme"] = "dark";
            _resolver.OptionAndValues["timeout"] = "123";
            _resolver.OptionAndValues["enabled"] = "true";

            _stringDef = new QkOptionDefinition { Name = "theme", Type = QkType.String };
            _numberDef = new QkOptionDefinition { Name = "timeout", Type = QkType.Number };
            _boolDef = new QkOptionDefinition { Name = "enabled", Type = QkType.Boolean };

            _stringExpr = new QkOptionExpression { Option = "theme", Operator = QkOperator.Equal, Value = "dark" };
            _numberExpr = new QkOptionExpression { Option = "timeout", Operator = QkOperator.GreaterEqual, Value = "99.9" };
            _boolExpr = new QkOptionExpression { Option = "enabled", Operator = QkOperator.Assignment, Value = "false" };
        }

        [Benchmark(Description = "Parse: Simple Assignment")]
        public QkOptionExpression? ParseSimple()
        {
            return _resolver.Parse(_simpleExpression);
        }

        [Benchmark(Description = "Parse: Complex Lookahead (>=)")]
        public QkOptionExpression? ParseComplex()
        {
            return _resolver.Parse(_complexExpression);
        }

        [Benchmark(Description = "Parse: No Operator (Worst Case Scan)")]
        public QkOptionExpression? ParseNoOperator()
        {
            return _resolver.Parse(_noOperatorExpression);
        }

        [Benchmark(Description = "Resolve: String Equality")]
        public bool ResolveString()
        {
            return _resolver.ResolveExpression(_stringExpr, _stringDef);
        }

        [Benchmark(Description = "Resolve: Number Inequality (Double.TryParse)")]
        public bool ResolveNumber()
        {
            return _resolver.ResolveExpression(_numberExpr, _numberDef);
        }

        [Benchmark(Description = "Resolve: Boolean Assignment")]
        public bool ResolveBool()
        {
            return _resolver.ResolveExpression(_boolExpr, _boolDef);
        }
    }
}
