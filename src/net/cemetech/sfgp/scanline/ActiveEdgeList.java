package net.cemetech.sfgp.scanline;

import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Comparator;
import java.util.Collections;

public class ActiveEdgeList {
	
	LinkedList<EdgeListDatum> activeEdges;
	int scanLine;
	
	public ActiveEdgeList(){ scanLine = -1; activeEdges = new LinkedList<EdgeListDatum>(); }
	public void stepEdges(HashSet<Primitive> activePrims){
		scanLine++;
		for(Iterator<EdgeListDatum> i = activeEdges.iterator(); i.hasNext();){
			EdgeListDatum edge = i.next();
			int edgeEnd = Math.max( 
					edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y),
					edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y));
			int lowEnd = Math.min( 
					edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y),
					edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y));
			if(edgeEnd < scanLine){
				i.remove();
				System.out.println("Deactivating " + edge.owner.color.toString() + 
						" with y-span: " + lowEnd + " -> " + edgeEnd + 
						" with x-span: " + edge.getMinXForLine(Projection.identity(), scanLine) + 
						" -> " + edge.getMaxXForLine(Projection.identity(), scanLine) + 
						"(true: " + edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X) + 
						" -> " + edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X) + ")");
			}
		}
		for(Primitive prim : activePrims){
			for(Edge e : prim.boundary){
				int sy = e.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y);
				int ey = e.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y);
				int mnY = Math.min(sy, ey);
				int mxY = Math.max(sy, ey);
				EdgeListDatum newEdge = new EdgeListDatum(e, prim);
				if (((sy == scanLine || ey == scanLine) && (sy != ey || newEdge.isSingleton())) || (scanLine == 0 && mnY < 0 && mxY > 0)){
					System.out.println("Activating " + prim.color.toString() + 
							" with y-span: " + mnY + " -> " + mxY + 
							" with x-span: " + newEdge.getMinXForLine(Projection.identity(), scanLine) + 
							" -> " + newEdge.getMaxXForLine(Projection.identity(), scanLine) + 
							"(true: " + e.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X) + 
							" -> " + e.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X) + ")");
					
					activeEdges.add(newEdge);
					if(activeEdges.getLast().isSingleton()){
						System.out.println("\t->Activating dummy end");
						activeEdges.add(new EdgeListDatum(e, prim, true));
					}
				}
			}
		}
		
		Collections.sort(activeEdges,new LeftToRight(scanLine));
		
	}

	public class EdgeListDatum{
		Edge edge;
		Primitive owner;
		boolean placeHolder;
		public EdgeListDatum(Edge e, Primitive p){this(e, p, false);}
		public EdgeListDatum(Edge e, Primitive p, boolean pH){
			edge = e; owner = p; placeHolder = pH;
		}
		
		public int getMinXForLine(Projection p, int scanLine){
			Edge projEdge = p.projectEdge(edge);
			Point start = projEdge.getEndPoint(Edge.EndPoint.START);
			Point end = projEdge.getEndPoint(Edge.EndPoint.END);
			int sx = start.getComponent(Point.CoordName.X);
			int ex = end.getComponent(Point.CoordName.X);
			int sy = start.getComponent(Point.CoordName.Y);
			int ey = end.getComponent(Point.CoordName.Y);
			int run = ex - sx;
			int rise = ey - sy;
			int ret = ((rise == 0) ? Math.min(ex, sx) : (((scanLine - sy) * run) / rise) + sx);
			return  ret;
		}
		
		public int getMaxXForLine(Projection p, int scanLine){
			Edge projEdge = p.projectEdge(edge);
			Point start = projEdge.getEndPoint(Edge.EndPoint.START);
			Point end = projEdge.getEndPoint(Edge.EndPoint.END);
			int sx = start.getComponent(Point.CoordName.X);
			int ex = end.getComponent(Point.CoordName.X);
			int sy = start.getComponent(Point.CoordName.Y);
			int ey = end.getComponent(Point.CoordName.Y);
			int rise = sy - ey;
			int run = sx - ex;
			int minBelow = getMinXForLine(p, scanLine - 1);
			int minAbove = getMinXForLine(p, scanLine + 1);
			return ((rise == 0) ? Math.max(sx, ex)
					: ((run == 0) ? sx : (Math.max(minBelow,minAbove) - 1)));
		}
		
		public int smartXForLine(Projection p, int scanLine){
			return placeHolder ? getMaxXForLine(p, scanLine) : getMinXForLine(p, scanLine);
		}
		
		public boolean isSingleton(){ return owner.getArity() == 2; }
	}
	
	public static class LeftToRight implements Comparator<EdgeListDatum>{
		
		int scanLine;
		private static Projection id = Projection.identity();
		public LeftToRight(int sL){scanLine = sL;}

		@Override
		public int compare(EdgeListDatum o1, EdgeListDatum o2) {
			return o1.smartXForLine(id, scanLine) - o2.smartXForLine(id, scanLine);
		}
		
	}
	
}


