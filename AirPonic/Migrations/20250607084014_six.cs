using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AirPonic.Migrations
{
    /// <inheritdoc />
    public partial class six : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "TotalLightTime",
                table: "MicroData");

            migrationBuilder.DropColumn(
                name: "LIGHT_HOURS_THRESHOLD",
                table: "MicroConfig");

            migrationBuilder.DropColumn(
                name: "LIGHT_INCREMENT",
                table: "MicroConfig");

            migrationBuilder.AlterColumn<string>(
                name: "DeviceId",
                table: "MicroData",
                type: "longtext",
                nullable: false,
                oldClrType: typeof(Guid),
                oldType: "char(36)")
                .Annotation("MySql:CharSet", "utf8mb4")
                .OldAnnotation("Relational:Collation", "ascii_general_ci");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<Guid>(
                name: "DeviceId",
                table: "MicroData",
                type: "char(36)",
                nullable: false,
                collation: "ascii_general_ci",
                oldClrType: typeof(string),
                oldType: "longtext")
                .OldAnnotation("MySql:CharSet", "utf8mb4");

            migrationBuilder.AddColumn<float>(
                name: "TotalLightTime",
                table: "MicroData",
                type: "float",
                nullable: false,
                defaultValue: 0f);

            migrationBuilder.AddColumn<int>(
                name: "LIGHT_HOURS_THRESHOLD",
                table: "MicroConfig",
                type: "int",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<float>(
                name: "LIGHT_INCREMENT",
                table: "MicroConfig",
                type: "float",
                nullable: false,
                defaultValue: 0f);
        }
    }
}
