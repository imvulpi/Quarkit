namespace Quarkit.Models.Core
{
    public class QkOptionExpression
    {
        public required string Option { get; set; }
        public required QkOperator Operator { get; set; }
        public required string Value { get; set; }
    }
}
