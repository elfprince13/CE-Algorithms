package net.cemetech.sfgp.scanline;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import net.cemetech.sfgp.scanline.geom.Edge;
import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Primitive;
import net.cemetech.sfgp.scanline.projections.Compose;
import net.cemetech.sfgp.scanline.projections.Orthographic;
import net.cemetech.sfgp.scanline.projections.Projection;
import net.cemetech.sfgp.scanline.projections.Scale;

public class RunTest {

	public static void main(String[] args){
		Point[][][] cubePoints = {{{new Point(0,0,0),new Point(0,0,1)},{new Point(0,1,0),new Point(0,1,1)}},
				{{new Point(1,0,0),new Point(1,0,1)},{new Point(1,1,0),new Point(1,1,1)}}};
		Edge[][] cubeEdges = {{
			new Edge(cubePoints[0][0][0],cubePoints[0][0][1]),
			new Edge(cubePoints[0][1][0],cubePoints[0][1][1]),
			new Edge(cubePoints[1][0][0],cubePoints[1][0][1]),
			new Edge(cubePoints[1][1][0],cubePoints[1][1][1]),
		},{
			new Edge(cubePoints[0][0][0],cubePoints[0][1][0]),
			new Edge(cubePoints[0][0][1],cubePoints[0][1][1]),
			new Edge(cubePoints[1][0][0],cubePoints[1][1][0]),
			new Edge(cubePoints[1][0][1],cubePoints[1][1][1]),
		},{
			new Edge(cubePoints[0][0][0],cubePoints[1][0][0]),
			new Edge(cubePoints[0][0][1],cubePoints[1][0][1]),
			new Edge(cubePoints[0][1][0],cubePoints[1][1][0]),
			new Edge(cubePoints[0][1][1],cubePoints[1][1][1]),
		}};
		
		Color[] faceColors = { Color.RED, Color.MAGENTA, Color.GREEN, Color.ORANGE, Color.YELLOW, Color.PINK, Color.BLUE };
		for(int i = 0; i < faceColors.length; i++){
			Color c = faceColors[i];
			int r = c.getRed();// >> 3;
			int g = c.getGreen();// >> 2;
			int b = c.getBlue();// >> 3;
			int rgb16 = (r << 16/*11*/ | g << 8/*5*/ | b);
			System.out.println(Integer.toHexString(rgb16));
		}
		System.exit(0);
		Primitive[] cube = {
				Primitive.makeQuad(cubeEdges[0][0], cubeEdges[1][0], cubeEdges[0][1], cubeEdges[1][1]),
				Primitive.makeQuad(cubeEdges[0][2], cubeEdges[1][3], cubeEdges[0][3], cubeEdges[1][2]),
				Primitive.makeQuad(cubeEdges[1][1], cubeEdges[2][3], cubeEdges[1][3], cubeEdges[2][1]),
				Primitive.makeQuad(cubeEdges[1][2], cubeEdges[2][2], cubeEdges[1][0], cubeEdges[2][0]),
				Primitive.makeQuad(cubeEdges[0][0], cubeEdges[2][1], cubeEdges[0][2], cubeEdges[2][0]),
				Primitive.makeQuad(cubeEdges[0][1], cubeEdges[2][2], cubeEdges[0][3], cubeEdges[2][3]),
				
		};
		for(int i = 0; i < cube.length; i++) cube[i].color = faceColors[i];
		
		//*
		Primitive[] cubeSkel = {
				Primitive.makeLine(cubeEdges[0][0]),
				Primitive.makeLine(cubeEdges[0][1]),
				Primitive.makeLine(cubeEdges[0][2]),
				Primitive.makeLine(cubeEdges[0][3]),
				Primitive.makeLine(cubeEdges[1][0]),
				Primitive.makeLine(cubeEdges[1][1]),
				Primitive.makeLine(cubeEdges[1][2]),
				Primitive.makeLine(cubeEdges[1][3]),
				Primitive.makeLine(cubeEdges[2][0]),
				Primitive.makeLine(cubeEdges[2][1]),
				Primitive.makeLine(cubeEdges[2][2]),
				Primitive.makeLine(cubeEdges[2][3]),
		};
		for(int i = 0; i < cubeSkel.length; i++) cubeSkel[i].color = Color.blue;
		//*/
		//*
		Primitive[] allPrims = new Primitive[cubeSkel.length + cube.length];
		int i = 0;
		for(Primitive p : cube){
			allPrims[i] = p;
			i++;
		}
		int j = 0;
		for(Primitive p : cubeSkel){
			allPrims[i+j] = p;
			j++;
		}
		//*/
		
		ScanlineRenderer<Projection> sr = new ScanlineRenderer<Projection>();
		
		int w = 320; int h = 240; int depth = 50;
		BufferedImage bi = new BufferedImage(w,h,BufferedImage.TYPE_INT_RGB);
		Graphics2D g = bi.createGraphics();
		g.setBackground(Color.white);
		g.clearRect(0, 0, w, h);
		Projection view = new Compose(new Projection(){

			@Override
			public Point projectPoint(Point p) {
				int x = p.getComponent(Point.CoordName.X);
				int y = p.getComponent(Point.CoordName.Y);
				int z = p.getComponent(Point.CoordName.Z);
				return new Point((int)(170 + x + 0.7*z), (int)(40 + y + 0.7*z), z);
			}
		
		},new Scale(100));
		//view = new Compose(new Orthographic(new Point(-h/2,-w/2,-depth),new Point(h/2,w/2,depth)), new Scale(25000));
		/*
		int k = 0;
		for(Primitive p: cube){
			for(Edge e : p.boundary){
				Point start = e.getEndPoint(Edge.EndPoint.START);
				Point end = e.getEndPoint(Edge.EndPoint.END);
				int sx = start.getComponent(Point.CoordName.X);
				int sy = start.getComponent(Point.CoordName.Y);
				int sz = start.getComponent(Point.CoordName.Z);
				int ex = end.getComponent(Point.CoordName.X);
				int ey = end.getComponent(Point.CoordName.Y);
				int ez = end.getComponent(Point.CoordName.Z);
				g.setColor(p.color);
				g.drawLine(
						50*k + (int)(20 * (sx + 0.7 * sz)), 
						(int)(20 * (sy + 0.7 * sz)), 
						50*k + (int)(20 * (ex + 0.7 * ez)), 
						(int)(20 * (ey + 0.7 * ez)) );
				
			}
			k++;
		}
		//*/
		/*
		for(Primitive p: cubeSkel){
			for(Edge e : p.boundary){
				Point start = e.getEndPoint(Edge.EndPoint.START);
				Point end = e.getEndPoint(Edge.EndPoint.END);
				int sx = start.getComponent(Point.CoordName.X);
				int sy = start.getComponent(Point.CoordName.Y);
				int sz = start.getComponent(Point.CoordName.Z);
				int ex = end.getComponent(Point.CoordName.X);
				int ey = end.getComponent(Point.CoordName.Y);
				int ez = end.getComponent(Point.CoordName.Z);
				g.setColor(p.color);
				g.drawLine(
						(int)(20 * (sx + 0.7 * sz)), 
						50 + (int)(20 * (sy + 0.7 * sz)), 
						(int)(20 * (ex + 0.7 * ez)), 
						50 + (int)(20 * (ey + 0.7 * ez)) );
				
			}
			k++;
		}
		//*/
		
		sr.render(bi, view, allPrims);
		
		JFrame jf = new JFrame("test");
		jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jf.add(new JLabel(new ImageIcon(bi)));
		jf.setVisible(true);
		jf.pack();
	}
}
