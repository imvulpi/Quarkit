using System.Text.Json;
using System.Text.Json.Serialization;

namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleDeltaConverter : JsonConverter<ModuleDelta>
    {
        public override ModuleDelta? Read(ref Utf8JsonReader reader, Type typeToConvert, JsonSerializerOptions options)
        {
            if (reader.TokenType == JsonTokenType.StartArray)
            {
                // Raw array so it's the single overwrite list.
                var list = JsonSerializer.Deserialize<List<string>>(ref reader, options);
                return new ModuleDelta { Values = list };
            }
            else if (reader.TokenType == JsonTokenType.StartObject)
            {
                // Object token, so it's a delta of Add/Remove lists
                using var jsonDoc = JsonDocument.ParseValue(ref reader);
                var root = jsonDoc.RootElement;

                var delta = new ModuleDelta();
                if (root.TryGetProperty("Add", out var addProp))
                    delta.Add = JsonSerializer.Deserialize<List<string>>(addProp.GetRawText(), options);
                if (root.TryGetProperty("Remove", out var removeProp))
                    delta.Remove = JsonSerializer.Deserialize<List<string>>(removeProp.GetRawText(), options);

                return delta;
            }

            throw new JsonException("Expected either a JSON Array or JSON Object for ModuleDelta configuration.");
        }

        public override void Write(Utf8JsonWriter writer, ModuleDelta value, JsonSerializerOptions options)
        {
            if (value.IsFullOverwrite)
            {
                JsonSerializer.Serialize(writer, value.Values, options);
            }
            else
            {
                writer.WriteStartObject();
                if (value.Add != null)
                {
                    writer.WritePropertyName("Add");
                    JsonSerializer.Serialize(writer, value.Add, options);
                }
                if (value.Remove != null)
                {
                    writer.WritePropertyName("Remove");
                    JsonSerializer.Serialize(writer, value.Remove, options);
                }
                writer.WriteEndObject();
            }
        }
    }
}
