library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sw_led is
	Port ( sw_in : in STD_LOGIC;
		led_out : out STD_LOGIC);
end sw_led;

architecture Behavioral of sw_led is
begin
	led_out <= not sw_in;
end Behavioral;
