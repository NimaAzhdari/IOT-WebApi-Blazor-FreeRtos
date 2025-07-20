using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AirPonic.Migrations
{
    /// <inheritdoc />
    public partial class second : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Date",
                table: "MicroData");

            migrationBuilder.DropColumn(
                name: "Time",
                table: "MicroData");

            migrationBuilder.AddColumn<Guid>(
                name: "DeviceId",
                table: "MicroData",
                type: "char(36)",
                nullable: false,
                defaultValue: new Guid("00000000-0000-0000-0000-000000000000"),
                collation: "ascii_general_ci");

            migrationBuilder.AddColumn<DateTime>(
                name: "Timestamp",
                table: "MicroData",
                type: "datetime(6)",
                nullable: false,
                defaultValue: new DateTime(1, 1, 1, 0, 0, 0, 0, DateTimeKind.Unspecified));
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "DeviceId",
                table: "MicroData");

            migrationBuilder.DropColumn(
                name: "Timestamp",
                table: "MicroData");

            migrationBuilder.AddColumn<DateOnly>(
                name: "Date",
                table: "MicroData",
                type: "date",
                nullable: false,
                defaultValue: new DateOnly(1, 1, 1));

            migrationBuilder.AddColumn<TimeOnly>(
                name: "Time",
                table: "MicroData",
                type: "time(6)",
                nullable: false,
                defaultValue: new TimeOnly(0, 0, 0));
        }
    }
}
