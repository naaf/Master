package robot;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

public class Srv {
	public static void main(String[] args) throws IOException {
		System.out.println("server demarre");
		ServerSocket s = new ServerSocket(2016);
		BufferedReader br;
		PrintStream pout;
		Socket sc = s.accept();
		br = new BufferedReader(new InputStreamReader(sc.getInputStream()));
		pout = new PrintStream(sc.getOutputStream());
		Scanner scane = new Scanner(System.in);
		String com = "SESSION";
		String corp = "SESSION/(3,4,H)(3,4,G)(12,6,H)(1,4,H)(9,4,G)(15,6,H)/";
		String r1 = com + '/' + corp + '/';
		String r2 = "SASOLUTION" + '/' + "ASHRAF" + '/' + "RDRHVDVHVDRB" + '/';
		String command;
		for (int i = 0; i < 3; i++) {
			System.out.println(br.readLine());
			System.out.println("entre votre commande :");
			command = scane.nextLine();
			System.out.println(command);
			pout.println(command);

		}

	}
}
