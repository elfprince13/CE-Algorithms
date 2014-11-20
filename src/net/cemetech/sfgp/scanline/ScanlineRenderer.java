package net.cemetech.sfgp.scanline;

import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.LinkedList;

import net.cemetech.sfgp.scanline.Edge.EndPoint;
import net.cemetech.sfgp.scanline.Point.CoordName;

public class ScanlineRenderer<T extends Projection> {
	static Projection id = Projection.identity();
	public void render(BufferedImage raster, T p, Primitive[] geometry){
		Primitive[] projectedGeometry = new Primitive[geometry.length];
		for(int i = 0; i < geometry.length; i++){
			projectedGeometry[i] = p.projectPrimitive(geometry[i]);
			System.out.println("Source geometry with arity " + geometry[i].getArity() + " begins on " + bottomMostPoint(geometry[i]) + " and ends on " + topMostPoint(geometry[i]));
			System.out.println("Projected geometry with arity " + projectedGeometry[i].getArity() + " begins on " + bottomMostPoint(projectedGeometry[i]) + " and ends on " + topMostPoint(projectedGeometry[i]));
		}
		
		int numLines = raster.getHeight();
		int lineWidth = raster.getWidth();
		ArrayList<HashSet<Primitive>> scanLinePrimBuckets = new ArrayList<HashSet<Primitive>>(numLines);
		for(int i = 0; i < numLines; i++) scanLinePrimBuckets.add(new HashSet<Primitive>());
		Arrays.sort(projectedGeometry,new TopToBottom());
		for(Primitive prim : projectedGeometry){
			int pScanline = bottomMostPoint(prim);
			if(pScanline < numLines && (pScanline >= 0 || topMostPoint(prim) >= 0)){
				int dstBucket = Math.max(0,pScanline);
				scanLinePrimBuckets.get(dstBucket).add(prim);
				System.out.println("dstBucket " + dstBucket + " gets geometry with arity " + prim.getArity() + " begins on " + bottomMostPoint(prim) + " and ends on " + topMostPoint(prim));
			}
		}
		
		HashSet<Primitive> activePrimSet = new HashSet<Primitive>();
		HashSet<Primitive> inFlags = new HashSet<Primitive>();
		ActiveEdgeList ael = new ActiveEdgeList();
		for(int line = 0; line < numLines; line++){
			System.out.println("Scanning line: " + line);
			if(!inFlags.isEmpty()){
				System.out.println("\tGarbage left in inFlags:");
				for(Primitive pp : inFlags){
					System.out.println("\t\t" + pp.color.toString());
				}
			}
			inFlags.clear();
			System.out.println("\tUpdating activePrimSet");
			for(Iterator<Primitive> i = activePrimSet.iterator(); i.hasNext();){
				Primitive prim = i.next();
				int top = topMostPoint(prim);
				int bottom = bottomMostPoint(prim);
				if(top < line){
					System.out.println("\t\t" + top + " -> " + bottom + " ( " + prim.color + " ) is not valid here: " + line);
					i.remove();	
				}
			}
			for(Primitive prim : scanLinePrimBuckets.get(line)){
				int top = topMostPoint(prim);
				int bottom = bottomMostPoint(prim);
				System.out.println("\t\t" + top + " -> " + bottom + " ( " + prim.color + " ) is added here: " + line);
				
				activePrimSet.add(prim);
			}
			ael.stepEdges(activePrimSet);
			int curPixel = 0;
			Primitive curDraw = null;
			Primitive oldDraw = null;
			ActiveEdgeList.EdgeListDatum startEdge;
			ActiveEdgeList.EdgeListDatum nextEdge;
			Iterator<ActiveEdgeList.EdgeListDatum> i = ael.activeEdges.iterator();
			if(i.hasNext()){
				nextEdge = i.next();
				while((nextEdge != null) && curPixel < lineWidth){
					startEdge = nextEdge;
					int startX = startEdge.smartXForLine(id, line);
					int nextX;
					if(i.hasNext()){
						nextEdge = i.next();
						nextX = nextEdge.smartXForLine(id, line);
						System.out.println("\tNext edges @ x = " + nextX + " from " + nextEdge.owner.color.toString());
					} else {
						System.out.println("\tNo more edges");
						nextEdge = null;
						nextX = 0;
					}
										
					if(inFlags.contains(startEdge.owner) ){
						System.out.println("\tNot *in* old " + startEdge.owner.color.toString() + " at " + startEdge.smartXForLine(id, line));
						inFlags.remove(startEdge.owner);
							oldDraw = (curDraw == null) ? startEdge.owner : curDraw;
					} else {
						System.out.println("\tNow *in* new " + startEdge.owner.color.toString() + " at " + startEdge.smartXForLine(id, line));
						inFlags.add(startEdge.owner);
					}
					
					if(curPixel < startX){
						System.out.println("curPixel has fallen behind, dragging from " + curPixel + " to " + startX);
						curPixel = startX;
					}

					if(nextEdge == null || nextX > curPixel){
						System.out.println("\tTesting depth:");
						curDraw = null; int bestZ = 0, j = 0;
						for(Primitive prim : inFlags){
							// The +1 business here is a hack, but most of this method will require
							// Some refactoring.
							int testZ = prim.getZForXY(curPixel+1, line);
							// -Z is out of the screen, so ... 
							if(j++ == 0 || testZ < bestZ){
								System.out.println("\t\tHit: " + testZ + " < " + bestZ + " || " + j + " == " + 1 + " for " + prim.color);
								bestZ = testZ;
								curDraw = prim;
							}
						}
					} else {
						System.out.println("\tSkipping depth test: " + nextEdge + ", " + nextX + ", " + curPixel);
					}

					if(oldDraw != null){
						raster.setRGB(curPixel++, line, oldDraw.color.getRGB());
						oldDraw = null;
					}
					if (curDraw != null) {
						if(nextEdge == null){
							System.out.println("Warning: we probably shouldn't have to draw if there are no edges to turn us off. Look for parity errors");
						}
						int drawWidth = ((nextEdge == null) ? lineWidth : nextX) - curPixel;
						drawWidth = Math.min(lineWidth - curPixel, Math.max(0, drawWidth));
						int[] rgbs = new int[drawWidth];
						Arrays.fill(rgbs, curDraw.color.getRGB());
						raster.setRGB(curPixel, line, drawWidth, 1, rgbs, 0, 1);
						curPixel +=  drawWidth;
					} else if (inFlags.size() == 0 && nextEdge != null) {
						System.out.println("Not in any polys at the moment, fast-forwarding to " + nextX);
						curPixel = nextX;
					}

				}
			}
		}
	}
	
	
	
	private class TopToBottom implements Comparator<Primitive> {	
		@Override
		public int compare(Primitive o1, Primitive o2) {
			return topMostPoint(o1) - topMostPoint(o2);
		}
	}
	
	private int topMostPoint(Primitive prim){
		int top = 0, i = 0;
		for(Edge e : prim.boundary) {
			int candidate = topMostPoint(e);
			if(i++ == 0 || candidate > top) top = candidate;
		}
		return top;
	}
	
	private int bottomMostPoint(Primitive prim){
		int bottom = 0, i = 0;
		for(Edge e : prim.boundary) {
			int candidate = bottomMostPoint(e);
			if(i++ == 0 || candidate < bottom) bottom = candidate;
		}
		return bottom;
	}
	
	private int topMostPoint(Edge e){
		return Math.max(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.Y),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.Y));
	}
	
	private int bottomMostPoint(Edge e){
		return Math.min(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.Y),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.Y));
	}
}
