library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity test_bench is
	Generic (CLOCK_RATE : integer := 200_000_000 -- clock rate
	);
end test_bench;

architecture Behavioral of test_bench is
	component sw_led is
	port ( sw_in : in STD_LOGIC;
		led_out : out STD_LOGIC);
	end component sw_led;
	signal clk_rx : std_logic := 'U';
	signal sw_in : std_logic := 'U';
	signal led_out : std_logic := '0';
begin

	-- welcome message
	welcome: process
	begin
		--writeNowToScreen("Starting the testbench for CQ SW-LED IP!");
		--writeNowToScreen("CLOCK_RATE = "&integer'IMAGE(CLOCK_RATE));
		wait;
	end process welcome;

	-- generate the clk_rx signal
	gen_clk_rx: process
	begin
		clk_rx <= '0';
		wait for 10ns;
		clk_rx <= '1';
		wait for 10ns;
	end process;

	sw_led_0: sw_led
		port map ( sw_in => sw_in,
		led_out => led_out );
	process begin
		wait for 10ns;

		sw_in <= '0';
		wait for 10ns;
		sw_in <= '1';
		wait for 10ns;
		sw_in <= '0';
		wait for 10ns;
		sw_in <= '1';
		wait;
	end process;
end Behavioral;

